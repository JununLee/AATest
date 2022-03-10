#include "vtkScreenPostProcessingPass.h"
#include <vtkObjectFactory.h>
#include <cassert>
#include <vtkRenderState.h>
#include <vtkRenderer.h>
#include <vtkOpenGLFramebufferObject.h>
#include <vtkTextureObject.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLError.h>
#include <vtkShaderProgram.h>
#include <vtkOpenGLShaderCache.h>
#include <vtkOpenGLState.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLHelper.h>


vtkScreenPostProcessingPass* vtkScreenPostProcessingPass::New(const char * vertexPath, const char * fragmentPath, const char * geometryPath)
{
	vtkScreenPostProcessingPass *result = new vtkScreenPostProcessingPass(vertexPath, fragmentPath, geometryPath);
	result->InitializeObjectBase();
	return result;
}

vtkScreenPostProcessingPass::vtkScreenPostProcessingPass(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	this->FrameBufferObject = nullptr;
	this->Pass = nullptr;
	this->Program = nullptr;
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;
	this->geometryPath = geometryPath;

	glGenTextures(1, &id);
}

vtkScreenPostProcessingPass::~vtkScreenPostProcessingPass()
{
	if (this->FrameBufferObject != nullptr)
	{
		vtkErrorMacro(<< "FrameBufferObject should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass != nullptr)
	{
		vtkErrorMacro(<< "Pass should have been deleted in ReleaseGraphicsResources().");
	}
}

void vtkScreenPostProcessingPass::PrintSelf(ostream & os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

void vtkScreenPostProcessingPass::Render(const vtkRenderState * s)
{
	assert("pre: s_exists" && s != nullptr);
	vtkOpenGLClearErrorMacro();
	this->NumberOfRenderedProps = 0;

	vtkRenderer* r = s->GetRenderer();
	vtkOpenGLRenderWindow* renWin = static_cast<vtkOpenGLRenderWindow*>(r->GetRenderWindow());
	vtkOpenGLState* openglstate = renWin->GetState();
	if (this->DelegatePass != nullptr) {
		int width;
		int height;
		int size[2];
		s->GetWindowSize(size);
		width = size[0];
		height = size[1];
	 	int* origin = r->GetOrigin();
		int w = width *2;
		int h = height*2;

		if (this->Pass == nullptr) {
			this->Pass = vtkTextureObject::New();
			this->Pass->SetContext(renWin);
		}
		if(this->FrameBufferObject==nullptr){
			this->FrameBufferObject = vtkOpenGLFramebufferObject::New();
			this->FrameBufferObject->SetContext(renWin);
		}
		vtkOpenGLState::ScopedglEnableDisable bsaver(openglstate, GL_BLEND);
		vtkOpenGLState::ScopedglEnableDisable dsaver(openglstate, GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		this->FrameBufferObject->SaveCurrentBindingsAndBuffers();
		this->RenderDelegate(s, w, h, w, h, this->FrameBufferObject, this->Pass);

		if (this->Pass->GetWidth() != static_cast<unsigned int>(w) ||this->Pass->GetHeight() != static_cast<unsigned int>(h))
		{
			this->Pass->Create2D(static_cast<unsigned int>(w),
				static_cast<unsigned int>(h), 3,
				VTK_UNSIGNED_CHAR, false);
		}

		this->FrameBufferObject->AddColorAttachment(
			this->FrameBufferObject->GetBothMode(), 0, this->Pass);
		this->FrameBufferObject->Start(w, h);


		if (!this->Program) {
			this->Program = new vtkOpenGLHelper;
			vtkShaderProgram *shader = loadShaderProgram(renWin);

			if (shader != this->Program->Program) {
				this->Program->Program = shader;
				this->Program->VAO->ShaderProgramChanged();
			}
			this->Program->ShaderSourceTime.Modified();
		}
		else
		{
			renWin->GetShaderCache()->ReadyShaderProgram(this->Program->Program);
		}

		if (!this->Program->Program || this->Program->Program->GetCompiled() != true) {
			vtkErrorMacro("Couldn't build the shader program. At this point , it can be an error in a shader or a driver bug.");
			this->FrameBufferObject->UnBind();
			this->FrameBufferObject->RestorePreviousBindingsAndBuffers();
			return;
		}

		this->Pass->Activate();
		int sourceId = this->Pass->GetTextureUnit();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (this->renderTex&&this->renderTex->data)
		{
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->renderTex->data);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			delete[] this->renderTex->data;
			this->renderTex->data = nullptr;
		}
		this->Program->Program->SetUniformi("source", sourceId); 
		this->Program->Program->SetUniformi("source1", id);

		openglstate->vtkglDisable(GL_BLEND);
		openglstate->vtkglDisable(GL_DEPTH_TEST);


		this->Pass->Deactivate();

		this->FrameBufferObject->UnBind();
		this->FrameBufferObject->RestorePreviousBindingsAndBuffers();


		float minXTexCoord = 0;
		float minYTexCoord = 0;

		float maxXTexCoord = 1;
		float maxYTexCoord = 1;

		vtkOpenGLState::ScopedglViewport vsaver(openglstate);
		openglstate->vtkglViewport(*origin, *(origin+1), width, height);

		float tcoords[] = {
			minXTexCoord, minYTexCoord,
			maxXTexCoord, minYTexCoord,
			maxXTexCoord, maxYTexCoord,
			minXTexCoord, maxYTexCoord };

		float verts[] = {
			-1.f, -1.f, 0.0f,
			1.0f, -1.f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.f, 1.0f, 0.0f };

		this->Pass->CopyToFrameBuffer(tcoords, verts, this->Program->Program, this->Program->VAO);

		vtkOpenGLCheckErrorMacro("failed after CopyToFrameBuffer")

		//this->Pass->CopyToFrameBuffer(0, 0, width, height, *origin, *(origin + 1), width , height, this->Program->Program, this->Program->VAO);
	}
	else
	{
		vtkWarningMacro(<< " no delegate.");
	}

	vtkOpenGLCheckErrorMacro("failed after Render");
}

void vtkScreenPostProcessingPass::ReleaseGraphicsResources(vtkWindow * w)
{
	assert("pre: w_exists" && w != nullptr);

	this->Superclass::ReleaseGraphicsResources(w);

	if (this->Program != nullptr)
	{
		this->Program->ReleaseGraphicsResources(w);
		delete this->Program;
		this->Program = nullptr;
	}
	if (this->FrameBufferObject != nullptr)
	{
		this->FrameBufferObject->Delete();
		this->FrameBufferObject = nullptr;
	}
	if (this->Pass != nullptr)
	{
		this->Pass->Delete();
		this->Pass = nullptr;
	}

}


vtkShaderProgram* vtkScreenPostProcessingPass::loadShaderProgram(vtkOpenGLRenderWindow * renWin)
{
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
		fragmentCode = fShaderStream.str();
		if (geometryPath != nullptr) {
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "failed read shader file: " << vertexPath << std::endl;
	}
	
	return renWin->GetShaderCache()->ReadyShaderProgram(vertexCode.c_str(), fragmentCode.c_str(), geometryCode.c_str());
}


