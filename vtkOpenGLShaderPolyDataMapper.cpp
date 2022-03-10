#include "vtkOpenGLShaderPolyDataMapper.h"
#include <vtkObjectFactory.h>
#include <vtkScalarsToColors.h>
#include <vtkOpenGLRenderTimer.h>
#include <vtkProperty.h>
#include <vtkOpenGLCamera.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLError.h>

vtkStandardNewMacro(vtkOpenGLShaderPolyDataMapper)

vtkOpenGLShaderPolyDataMapper::vtkOpenGLShaderPolyDataMapper()
{
	ZWrite = GL_TRUE;
	DepthFunc =  GL_LEQUAL;
}

vtkOpenGLShaderPolyDataMapper::~vtkOpenGLShaderPolyDataMapper()
{
}

void vtkOpenGLShaderPolyDataMapper::UseShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {

	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		this->SetVertexShaderCode(vertexCode.c_str());
		fragmentCode = fShaderStream.str();
		this->SetFragmentShaderCode(fragmentCode.c_str());
		if (geometryPath != nullptr) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
			this->SetGeometryShaderCode(geometryCode.c_str());
		}
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "failed read shader file: " << vertexPath << std::endl;
	}
}

void vtkOpenGLShaderPolyDataMapper::Render(vtkRenderer* ren, vtkActor* act) {
	glBindTexture(GL_TEXTURE_2D, 0);

	for (auto val : textures)
	{
		val.second->ReleaseGraphicsResources(ren->GetVTKWindow());
		val.second->Render(ren);
	}
	for (auto var : volumeTex)
	{

	}
	if (this->Static)
	{
		this->RenderPiece(ren, act);
		return;
	}

	vtkInformation *inInfo = this->GetInputInformation();
	if (inInfo == nullptr)
	{
		vtkErrorMacro("Mapper has no input.");
		return;
	}

	openglState = static_cast<vtkOpenGLRenderer *>(ren)->GetState();
	SetOpenGLState();


	int nPieces = this->NumberOfPieces * this->NumberOfSubPieces;
	for (int i = 0; i < this->NumberOfSubPieces; i++)
	{
		// If more than one pieces, render in loop.
		int currentPiece = this->NumberOfSubPieces * this->Piece + i;
		this->GetInputAlgorithm()->UpdateInformation();
		inInfo->Set(
			vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(), currentPiece);
		inInfo->Set(
			vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(), nPieces);
		inInfo->Set(
			vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(),
			this->GhostLevel);
		this->RenderPiece(ren, act);
	}
	ResetOpenGLState();
}

void vtkOpenGLShaderPolyDataMapper::UpdateShaders(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor) {
	vtkOpenGLRenderWindow *renWin = vtkOpenGLRenderWindow::SafeDownCast(ren->GetRenderWindow());

	cellBO.VAO->Bind();
	this->LastBoundBO = &cellBO;
	// has something changed that would require us to recreate the shader?
	if (this->GetNeedToRebuildShaders(cellBO, ren, actor))
	{
		// build the shader source code
		std::map<vtkShader::Type, vtkShader *> shaders;
		vtkShader *vss = vtkShader::New();
		vss->SetType(vtkShader::Vertex);
		shaders[vtkShader::Vertex] = vss;
		vtkShader *gss = vtkShader::New();
		gss->SetType(vtkShader::Geometry);
		shaders[vtkShader::Geometry] = gss;
		vtkShader *fss = vtkShader::New();
		fss->SetType(vtkShader::Fragment);
		shaders[vtkShader::Fragment] = fss;

		this->BuildShaders(shaders, ren, actor);

		// compile and bind the program if needed
		shaderProgram = renWin->GetShaderCache()->ReadyShaderProgram(shaders);


		vss->Delete();
		fss->Delete();
		gss->Delete();

		// if the shader changed reinitialize the VAO
		if (shaderProgram != cellBO.Program)
		{
			cellBO.Program = shaderProgram;
			// reset the VAO as the shader has changed
			cellBO.VAO->ReleaseGraphicsResources();
		}
		cellBO.ShaderSourceTime.Modified();
	}
	else
	{
		renWin->GetShaderCache()->ReadyShaderProgram(cellBO.Program);
	}

	if (cellBO.Program)
	{
		this->SetMapperShaderParameters(cellBO, ren, actor);
		this->SetPropertyShaderParameters(cellBO, ren, actor);
		this->SetCameraShaderParameters(cellBO, ren, actor);
		this->SetLightingShaderParameters(cellBO, ren, actor);

		// allow the program to set what it wants
		this->InvokeEvent(vtkCommand::UpdateShaderEvent, cellBO.Program);
	}

	vtkOpenGLCheckErrorMacro("failed after UpdateShader");
}

void vtkOpenGLShaderPolyDataMapper::SetMapperShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor)
{

	// Now to update the VAO too, if necessary.
	cellBO.Program->SetUniformi("PrimitiveIDOffset",
		this->PrimitiveIDOffset);

	if (cellBO.IBO->IndexCount &&
		(this->VBOs->GetMTime() > cellBO.AttributeUpdateTime ||
			cellBO.ShaderSourceTime > cellBO.AttributeUpdateTime))
	{
		cellBO.VAO->Bind();

		this->VBOs->AddAllAttributesToVAO(cellBO.Program, cellBO.VAO);

		if (!this->AppleBugPrimIDs.empty() &&
			cellBO.Program->IsAttributeUsed("appleBugPrimID"))
		{
			if (!cellBO.VAO->AddAttributeArray(cellBO.Program,
				this->AppleBugPrimIDBuffer,
				"appleBugPrimID",
				0, sizeof(float), VTK_UNSIGNED_CHAR, 4, true))
			{
				vtkErrorMacro(<< "Error setting 'appleBugPrimID' in shader VAO.");
			}
		}

		cellBO.AttributeUpdateTime.Modified();
	}

	if (volumeTex.size() != 0) {
		for (auto var : volumeTex) {
			auto block = var.second->GetCurrentBlock();
			block->TextureObject->Activate();
			cellBO.Program->SetUniformi(var.first.c_str(), block->TextureObject->GetTextureUnit());
			cellBO.Program->SetUniformf("scale", var.second->Scale[0]);
			cellBO.Program->SetUniformf("bias", var.second->Bias[0]);

		}
	}

	//if (this->HaveTextures(actor))
	if (textures.size() != 0)
	{
		/*std::vector<texinfo> textures = this->GetTextures(actor);
		for (size_t i = 0; i < textures.size(); ++i)
		{
		vtkTexture *texture = textures[i].first;
		if (texture && cellBO.Program->IsUniformUsed(textures[i].second.c_str()))
		{
		int tunit = vtkOpenGLTexture::SafeDownCast(texture)->GetTextureUnit();
		cellBO.Program->SetUniformi(textures[i].second.c_str(), tunit);
		}
		}*/
		for (auto val : textures)
		{
			vtkTexture *texture = val.second;
			if (texture && cellBO.Program->IsUniformUsed(val.first.c_str()))
			{
				int tunit = vtkOpenGLTexture::SafeDownCast(texture)->GetTextureUnit();
				cellBO.Program->SetUniformi(val.first.c_str(), tunit);
			}
		}
		// check for tcoord transform matrix
		vtkInformation *info = actor->GetPropertyKeys();
		vtkOpenGLCheckErrorMacro("failed after Render");
		if (info && info->Has(vtkProp::GeneralTextureTransform()) &&
			cellBO.Program->IsUniformUsed("tcMatrix"))
		{
			double *dmatrix = info->Get(vtkProp::GeneralTextureTransform());
			float fmatrix[16];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					fmatrix[j * 4 + i] = dmatrix[i * 4 + j];
				}
			}
			cellBO.Program->SetUniformMatrix4x4("tcMatrix", fmatrix);
			vtkOpenGLCheckErrorMacro("failed after Render");
		}
	}
	if ((this->HaveCellScalars) &&
		cellBO.Program->IsUniformUsed("textureC"))
	{
		int tunit = this->CellScalarTexture->GetTextureUnit();
		cellBO.Program->SetUniformi("textureC", tunit);
	}

	if (this->HaveCellNormals && cellBO.Program->IsUniformUsed("textureN"))
	{
		int tunit = this->CellNormalTexture->GetTextureUnit();
		cellBO.Program->SetUniformi("textureN", tunit);
	}

	// Handle render pass setup:
	vtkInformation *info = actor->GetPropertyKeys();
	if (info && info->Has(vtkOpenGLRenderPass::RenderPasses()))
	{
		int numRenderPasses = info->Length(vtkOpenGLRenderPass::RenderPasses());
		for (int i = 0; i < numRenderPasses; ++i)
		{
			vtkObjectBase *rpBase = info->Get(vtkOpenGLRenderPass::RenderPasses(), i);
			vtkOpenGLRenderPass *rp = static_cast<vtkOpenGLRenderPass*>(rpBase);
			if (!rp->SetShaderParameters(cellBO.Program, this, actor, cellBO.VAO))
			{
				vtkErrorMacro("RenderPass::SetShaderParameters failed for renderpass: "
					<< rp->GetClassName());
			}
		}
	}

	vtkHardwareSelector* selector = ren->GetSelector();
	if (selector && cellBO.Program->IsUniformUsed("mapperIndex"))
	{
		cellBO.Program->SetUniform3f("mapperIndex", selector->GetPropColorValue());
	}

	if (this->GetNumberOfClippingPlanes() &&
		cellBO.Program->IsUniformUsed("numClipPlanes") &&
		cellBO.Program->IsUniformUsed("clipPlanes"))
	{
		// add all the clipping planes
		int numClipPlanes = this->GetNumberOfClippingPlanes();
		if (numClipPlanes > 6)
		{
			vtkErrorMacro(<< "OpenGL has a limit of 6 clipping planes");
			numClipPlanes = 6;
		}

		double shift[3] = { 0.0, 0.0, 0.0 };
		double scale[3] = { 1.0, 1.0, 1.0 };
		vtkOpenGLVertexBufferObject *vvbo = this->VBOs->GetVBO("vertexMC");
		if (vvbo && vvbo->GetCoordShiftAndScaleEnabled())
		{
			const std::vector<double> &vh = vvbo->GetShift();
			const std::vector<double> &vc = vvbo->GetScale();
			for (int i = 0; i < 3; ++i)
			{
				shift[i] = vh[i];
				scale[i] = vc[i];
			}
		}

		float planeEquations[6][4];
		for (int i = 0; i < numClipPlanes; i++)
		{
			double planeEquation[4];
			this->GetClippingPlaneInDataCoords(actor->GetMatrix(), i, planeEquation);

			// multiply by shift scale if set
			planeEquations[i][0] = planeEquation[0] / scale[0];
			planeEquations[i][1] = planeEquation[1] / scale[1];
			planeEquations[i][2] = planeEquation[2] / scale[2];
			planeEquations[i][3] = planeEquation[3]
				+ planeEquation[0] * shift[0]
				+ planeEquation[1] * shift[1]
				+ planeEquation[2] * shift[2];
		}
		cellBO.Program->SetUniformi("numClipPlanes", numClipPlanes);
		cellBO.Program->SetUniform4fv("clipPlanes", 6, planeEquations);
	}

	// handle wide lines
	if (this->HaveWideLines(ren, actor) &&
		cellBO.Program->IsUniformUsed("lineWidthNVC"))
	{
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		float lineWidth[2];
		lineWidth[0] = 2.0*actor->GetProperty()->GetLineWidth() / vp[2];
		lineWidth[1] = 2.0*actor->GetProperty()->GetLineWidth() / vp[3];
		cellBO.Program->SetUniform2f("lineWidthNVC", lineWidth);
	}

	// handle coincident
	if (cellBO.Program->IsUniformUsed("coffset"))
	{
		float factor, offset;
		this->GetCoincidentParameters(ren, actor, factor, offset);
		cellBO.Program->SetUniformf("coffset", offset);
		// cfactor isn't always used when coffset is.
		if (cellBO.Program->IsUniformUsed("cfactor"))
		{
			cellBO.Program->SetUniformf("cfactor", factor);
		}
	}
}

void vtkOpenGLShaderPolyDataMapper::SetPropertyShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor) {

	SetShaderParama(vec1, f);
	SetShaderParama(vec2, 2f);
	SetShaderParama(vec3, 3f);
	SetShaderParama(vec4, 4f);
	SetShaderParama(dou3, 3f);
	SetShaderParama(int1, i);
	SetShaderParama(mat3x3, Matrix3x3);
	SetShaderParama(mat4x4, Matrix4x4);
	for (auto var : vtkMat3x3) {
		vtkMatrix3x3* out = vtkMatrix3x3::New();
		vtkMatrix3x3::Transpose(var.second, out);
		shaderProgram->SetUniformMatrix(var.first.c_str(), out);
	}
	for (auto var : vtkMat4x4) {
		vtkMatrix4x4* out = vtkMatrix4x4::New();
		vtkMatrix4x4::Transpose(var.second, out);
		shaderProgram->SetUniformMatrix(var.first.c_str(), out);

	}
}

void vtkOpenGLShaderPolyDataMapper::SetCameraShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor)
{
	vtkShaderProgram *program = cellBO.Program;

	vtkOpenGLCamera *cam = (vtkOpenGLCamera *)(ren->GetActiveCamera());

	// [WMVD]C == {world, model, view, display} coordinates
	// E.g., WCDC == world to display coordinate transformation
	vtkMatrix4x4* wcdc;
	vtkMatrix4x4* wcvc;
	vtkMatrix3x3* norms;
	vtkMatrix4x4* vcdc;
	cam->GetKeyMatrices(ren, wcvc, norms, vcdc, wcdc);

	if (program->IsUniformUsed("ZCalcR"))
	{
		if (cam->GetParallelProjection())
		{
			program->SetUniformf("ZCalcS", vcdc->GetElement(2, 2));
		}
		else
		{
			program->SetUniformf("ZCalcS", -0.5*vcdc->GetElement(2, 2) + 0.5);
		}
		if (this->DrawingSpheres(cellBO, actor))
		{
			program->SetUniformf("ZCalcR",
				actor->GetProperty()->GetPointSize() /
				(ren->GetSize()[0] * vcdc->GetElement(0, 0)));
		}
		else
		{
			program->SetUniformf("ZCalcR",
				actor->GetProperty()->GetLineWidth() /
				(ren->GetSize()[0] * vcdc->GetElement(0, 0)));
		}
	}

	// If the VBO coordinates were shifted and scaled, apply the inverse transform
	// to the model->view matrix:
	vtkOpenGLVertexBufferObject *vvbo = this->VBOs->GetVBO("vertexMC");
	if (vvbo && vvbo->GetCoordShiftAndScaleEnabled())
	{
		if (!actor->GetIsIdentity())
		{
			vtkMatrix4x4* mcwc;
			vtkMatrix3x3* anorms;
			static_cast<vtkOpenGLActor *>(actor)->GetKeyMatrices(mcwc, anorms);
			vtkMatrix4x4::Multiply4x4(this->VBOShiftScale, mcwc, this->TempMatrix4);
			vtkMatrix4x4::Multiply4x4(this->TempMatrix4, wcdc, this->TempMatrix4);
			program->SetUniformMatrix("MCDCMatrix", this->TempMatrix4);
			if (program->IsUniformUsed("MCVCMatrix"))
			{
				vtkMatrix4x4::Multiply4x4(this->VBOShiftScale, mcwc, this->TempMatrix4);
				vtkMatrix4x4::Multiply4x4(this->TempMatrix4, wcvc, this->TempMatrix4);
				program->SetUniformMatrix("MCVCMatrix", this->TempMatrix4);
			}
			if (program->IsUniformUsed("normalMatrix"))
			{
				vtkMatrix3x3::Multiply3x3(anorms, norms, this->TempMatrix3);
				program->SetUniformMatrix("normalMatrix", this->TempMatrix3);
			}
		}
		else
		{
			vtkMatrix4x4::Multiply4x4(this->VBOShiftScale, wcdc, this->TempMatrix4);
			program->SetUniformMatrix("MCDCMatrix", this->TempMatrix4);
			if (program->IsUniformUsed("MCVCMatrix"))
			{
				vtkMatrix4x4::Multiply4x4(this->VBOShiftScale, wcvc, this->TempMatrix4);
				program->SetUniformMatrix("MCVCMatrix", this->TempMatrix4);
			}
			if (program->IsUniformUsed("normalMatrix"))
			{
				program->SetUniformMatrix("normalMatrix", norms);
			}
		}
	}
	else
	{
		if (!actor->GetIsIdentity())
		{
			vtkMatrix4x4 *mcwc;
			vtkMatrix3x3 *anorms;
			((vtkOpenGLActor *)actor)->GetKeyMatrices(mcwc, anorms);
			if (program->IsUniformUsed("MCWCMatrix")) {
				program->SetUniformMatrix("MCWCMatrix", mcwc);
			}
			vtkMatrix4x4::Multiply4x4(mcwc, wcdc, this->TempMatrix4);
			program->SetUniformMatrix("MCDCMatrix", this->TempMatrix4);
			if (program->IsUniformUsed("MCVCMatrix"))
			{
				vtkMatrix4x4::Multiply4x4(mcwc, wcvc, this->TempMatrix4);
				program->SetUniformMatrix("MCVCMatrix", this->TempMatrix4);
			}
			if (program->IsUniformUsed("normalMatrix"))
			{
				vtkMatrix3x3::Multiply3x3(anorms, norms, this->TempMatrix3);
				program->SetUniformMatrix("normalMatrix", this->TempMatrix3);
			}
		}
		else
		{
			program->SetUniformMatrix("MCDCMatrix", wcdc);
			if (program->IsUniformUsed("MCVCMatrix"))
			{
				program->SetUniformMatrix("MCVCMatrix", wcvc);
			}
			if (program->IsUniformUsed("normalMatrix"))
			{
				program->SetUniformMatrix("normalMatrix", norms);
			}
		}
	}

	if (program->IsUniformUsed("cameraParallel"))
	{
		program->SetUniformi("cameraParallel", cam->GetParallelProjection());
	}
}

void vtkOpenGLShaderPolyDataMapper::SetOpenGLState() {
	openglState->vtkglEnable(GL_CULL_FACE);
	openglState->vtkglCullFace(GL_BACK);
	if (!CullFace) {
		openglState->vtkglDisable(GL_CULL_FACE);
	}
	if (CullFront) {
		openglState->vtkglCullFace(GL_FRONT);
	}
	openglState->vtkglDepthFunc(DepthFunc);
	openglState->vtkglEnable(GL_BLEND);
	if (!Blend)
		openglState->vtkglDisable(GL_BLEND);
	openglState->vtkglDepthMask(ZWrite);

	openglState->vtkglColorMask(ColorMask,ColorMask,ColorMask,ColorMask);

	//glEnable(GL_MULTISAMPLE);
}

void vtkOpenGLShaderPolyDataMapper::ResetOpenGLState() {
	openglState->vtkglEnable(GL_CULL_FACE);
	openglState->vtkglCullFace(GL_BACK);
	openglState->vtkglColorMask(true, true, true, true);
	openglState->vtkglDepthFunc(GL_LEQUAL);
}