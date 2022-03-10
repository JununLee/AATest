
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkJPEGReader.h>
#include <vtkTexture.h>
#include "vtkOpenGLShaderPolyDataMapper.h"
#include <vtk_glew.h>
#include <vtkRenderStepsPass.h>
#include <vtkSimpleMotionBlurPass.h>
#include <vtkOpenGLRenderer.h>
#include <vtkGaussianBlurPass.h>
#include <vtkFramebufferPass.h>
#include <vtkDICOMImageReader.h>
#include <vtkVolumeTexture.h>
#include <vtkImageData.h>
#include <vtkDataArray.h>
#include <vtkAbstractArray.h>
#include <vtkAbstractMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeInputHelper.h>
#include <vtkTimerLog.h>
#include <vtkPlaneSource.h>
#include "vtkScreenPostProcessingPass.h"
#include <vtkTransform.h>
#include <vtkOpenGLActor.h>
#include "vtkFramTextureOpenGLRender.h"

int main123() {

	vtkSmartPointer<vtkPlaneSource> planeSource = vtkSmartPointer<vtkPlaneSource>::New();
	vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();

	vtkSmartPointer<vtkOpenGLShaderPolyDataMapper> mapper = vtkSmartPointer<vtkOpenGLShaderPolyDataMapper>::New();
	mapper->SetInputConnection(cubeSource->GetOutputPort());
	vtkSmartPointer<vtkOpenGLShaderPolyDataMapper> mapper1 = vtkSmartPointer<vtkOpenGLShaderPolyDataMapper>::New();
	mapper1->SetInputConnection(planeSource->GetOutputPort());
	vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper2->SetInputConnection(cubeSource->GetOutputPort());

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetPosition(0, 0, 0);
	actor->SetScale(2);
	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);
	actor1->SetPosition(0, 0, -1.5);
	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->SetMapper(mapper2);
	//actor2->SetScale(0.6, 0.6, 2);
	//actor2->AddPosition(0, 0, -1);
	actor2->RotateX(90);
	actor2->GetProperty()->SetRepresentationToWireframe();
	actor2->GetProperty()->SetAmbientColor(0, 1, 0);
	actor2->GetProperty()->SetColor(0, 1, 0);
	actor2->GetProperty()->SetLighting(false);
	actor2->GetProperty()->SetDiffuseColor(0, 1, 0);
	actor2->GetProperty()->SetLineWidth(5);


	mapper->UseShader("C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkXrayShader.vs", "C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkXrayShader.fs");

	mapper1->UseShader("C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkShader.vs", "C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkShader.fs");

	float color[4] = { 0,1,1,1 };
	mapper1->SetVec4("color", color);

	float mat[9] = { 0,-1,0,1,0,0,0,0,1 };
	mapper1->SetMat3x3("rot", mat);

	vtkSmartPointer<vtkJPEGReader> jpgreader = vtkSmartPointer<vtkJPEGReader>::New();
	jpgreader->SetFileName("C:\\Users\\PDC-48\\Desktop\\JununLee.jpg");
	vtkSmartPointer<vtkTexture> vtktex = vtkSmartPointer<vtkTexture>::New();
	vtktex->SetInputConnection(jpgreader->GetOutputPort());

	vtkSmartPointer<vtkJPEGReader> jpgreader1 = vtkSmartPointer<vtkJPEGReader>::New();
	jpgreader1->SetFileName("C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Image\\container.jpg");
	vtkSmartPointer<vtkTexture> vtktex1 = vtkSmartPointer<vtkTexture>::New();
	vtktex1->SetInputConnection(jpgreader1->GetOutputPort());

	vtkSmartPointer<vtkDICOMImageReader> volumeReader = vtkSmartPointer<vtkDICOMImageReader>::New();
	volumeReader->SetDirectoryName("E:\\Data\\lxm");
	volumeReader->Update();
	auto input = volumeReader->GetOutput();


	//vtkSmartPointer<vtkVolumeTexture> texture = vtkSmartPointer<vtkVolumeTexture>::New();
	//vtkVolumeInputHelper currentInput;
	//currentInput.Texture = texture;
	//auto& volInput = currentInput;
	//auto volumeTex = volInput.Texture.GetPointer();
	//volumeTex->SetPartitions(1, 1, 1);
	//int cellFlag;
	//vtkAbstractArray* abstractScalars = vtkAbstractMapper::GetAbstractScalars(input, 0, 0, -1, "", cellFlag);
	//vtkDataArray* scalars = vtkArrayDownCast<vtkDataArray>(abstractScalars);


	//mapper->SetTexture("myTex", texture);
	//mapper1->SetTexture("myTex", vtktex1);

	mapper->CullFace = false;
	mapper1->CullFace = false;
	//mapper->CullFront = true;
	mapper->ZWrite = false;

	vtkSmartPointer<vtkOpenGLRenderer> render = vtkSmartPointer<vtkOpenGLRenderer>::New();
	vtkSmartPointer<vtkFramTextureOpenGLRender> render1 = vtkSmartPointer<vtkFramTextureOpenGLRender>::New();
	vtkSmartPointer<vtkOpenGLRenderer> render2 = vtkSmartPointer<vtkOpenGLRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	window->AddRenderer(render1);
	window->AddRenderer(render);
	window->AddRenderer(render2);
	interactor->SetRenderWindow(window);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);

	render->AddActor(actor);
	//render->AddActor(actor2);
	render->SetBackground(0.2, 0.2, 0.4);
	render->SetViewport(0, 0, 0.5, 1);
	render->GetActiveCamera()->SetPosition(0, 0, 1);
	//ender->GetActiveCamera()->SetClippingRange(0.01, 1000);

	render1->AddActor(actor2);
	render1->SetBackground(0.5, 0.7, 0);
	render1->SetViewport(0.5, 0, 1, 1);
	render1->GetActiveCamera()->SetPosition(0, 0, 1);

	//render1->GetActiveCamera()->SetClippingRange(0.01, 1000);

	render2->AddActor(actor1);
	render2->SetBackground(0, 0, 0.4);
	render2->SetViewport(1, 0, 1, 1);
	render2->GetActiveCamera()->SetPosition(0, 0, 1);

	//render2->GetActiveCamera()->SetClippingRange(0.01, 1000);

	window->SetSize(1600, 800);
	window->SetWindowName("OpenGL");

	
	
	vtkNew<vtkRenderStepsPass> basicPasses;
	vtkSmartPointer<vtkScreenPostProcessingPass> motion = vtkScreenPostProcessingPass::New(
		"C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkScreenShader.vs",
		"C:\\Users\\PDC-48\\Desktop\\LearnOpenGL\\Shader\\vtkScreenShader.fs");
	motion->renderTex = render1;
	motion->SetDelegatePass(basicPasses);
	vtkOpenGLRenderer *glrenderer = vtkOpenGLRenderer::SafeDownCast(render);
	//vtkOpenGLRenderer *glrenderer1 = vtkOpenGLRenderer::SafeDownCast(render1);
	glrenderer->SetPass(motion);
	//glrenderer1->SetPass(motion1);

	interactor->Initialize();

	//volumeTex->LoadVolume(render, input, scalars, cellFlag, 1);


	vtkSmartPointer<vtkVolumeTexture> texture = vtkSmartPointer<vtkVolumeTexture>::New();
	int cellFlag;
	vtkAbstractArray* abstractScalars = vtkAbstractMapper::GetAbstractScalars(input, 0, 0, -1, "", cellFlag);
	vtkDataArray* scalars = vtkArrayDownCast<vtkDataArray>(abstractScalars);
	texture->LoadVolume(render2, input, scalars, cellFlag, 1);

	float sx = texture->CellSpacing[0] * texture->GetCurrentBlock()->TextureSize[0] / 1000;
	float sy = texture->CellSpacing[1] * texture->GetCurrentBlock()->TextureSize[1] / 1000;
	float sz = texture->CellSpacing[2] * texture->GetCurrentBlock()->TextureSize[2] / 1000;
	actor2->SetScale(sx, sy, sz);

	mapper->SetVolumeTexture("myTex", texture);
	mapper->SetVec3("cameraPos", render->GetActiveCamera()->GetPosition());
	vtkMatrix4x4* mat1 = actor->GetMatrix();
	vtkMatrix4x4* mat2 = actor2->GetMatrix();
	mapper->SetMat4x4("local2world", mat1);
	mapper->SetMat4x4("boxlocal2world", mat2); 
	mapper->SetVec1("minDataValue", texture->ScalarRange[0][0]);
	mapper->SetVec1("maxDataValue", texture->ScalarRange[0][1]);
 	mapper1->SetVolumeTexture("myTex", texture);
	interactor->Render();
	interactor->Start();

	return EXIT_SUCCESS;
}