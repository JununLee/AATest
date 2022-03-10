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
#include <vtkImageMapper.h>
#include <vtkImageActor.h>
#include <vtkCylinderSource.h>

int main_N() {

	vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
	reader->SetFileName("C:\\Users\\dell\\Desktop\\JununLee.jpg");
	reader->Update();

	vtkSmartPointer<vtkImageMapper> m = vtkSmartPointer<vtkImageMapper>::New();
	m->SetInputData(reader->GetOutput());
	m->SetColorWindow(256);
	m->SetColorLevel(128);

	vtkSmartPointer<vtkImageActor> imgActor = vtkSmartPointer<vtkImageActor>::New();
	imgActor->SetInputData(reader->GetOutput());



	vtkSmartPointer<vtkCylinderSource> cylineSource = vtkSmartPointer<vtkCylinderSource>::New();
	cylineSource->SetRadius(10);
	cylineSource->SetResolution(50);
	cylineSource->SetHeight(100);
	vtkSmartPointer<vtkOpenGLShaderPolyDataMapper> mapper = vtkSmartPointer<vtkOpenGLShaderPolyDataMapper>::New();
	mapper->SetInputConnection(cylineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetPosition(300, 300, 0);
	actor->RotateX(-45);

	vtkSmartPointer<vtkOpenGLShaderPolyDataMapper> mapper1 = vtkSmartPointer<vtkOpenGLShaderPolyDataMapper>::New();
	mapper1->SetInputConnection(cylineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->SetMapper(mapper1);
	actor1->SetPosition(300, 300, 0);
	actor1->RotateX(-45);

	vtkSmartPointer<vtkOpenGLShaderPolyDataMapper> mapper2 = vtkSmartPointer<vtkOpenGLShaderPolyDataMapper>::New();
	mapper2->SetInputConnection(cylineSource->GetOutputPort());
	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->SetMapper(mapper2);
	actor2->SetPosition(300, 300, 0);
	actor2->RotateX(-45);

	mapper->UseShader("C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkNailShader.vs", "C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkNailShader.fs");

	mapper->ZWrite = false;
	mapper->DepthFunc = GL_ALWAYS;
	mapper->Blend = true;

	mapper1->UseShader("C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkColorMask.vs", "C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkColorMask.fs");
	mapper1->ColorMask = false;

	mapper2->UseShader("C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkSection.vs", "C:\\Users\\dell\\Desktop\\LearnOpenGL\\Shader\\vtkSection.fs");

	mapper2->DepthFunc = GL_LESS;
	mapper2->Blend = true;
	mapper2->CullFront = true;

	vtkSmartPointer<vtkOpenGLRenderer> render = vtkSmartPointer<vtkOpenGLRenderer>::New();
	render->AddActor(imgActor);
	
	render->SetBackground(0, 0, 0);
	//render->ResetCamera();
	render->GetActiveCamera()->ParallelProjectionOn();
	render->GetActiveCamera()->SetClippingRange(0.01, 3000.01);
	render->GetActiveCamera()->SetThickness(3000);
	render->GetActiveCamera()->SetParallelScale(256);
	render->GetActiveCamera()->SetFocalPoint(256, 256,0);
	render->GetActiveCamera()->SetPosition(256, 256,600);
	render->AddActor(actor);
	render->AddActor(actor1);
	render->AddActor(actor2);



	vtkSmartPointer<vtkRenderWindow> rw = vtkSmartPointer<vtkRenderWindow>::New();
	rw->AddRenderer(render);
	rw->SetWindowName("MeasurementDistanceApp");
	rw->SetSize(800, 800);
	rw->Render();

	vtkSmartPointer<vtkRenderWindowInteractor> rwi = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	rwi->SetRenderWindow(rw);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

	rwi->SetInteractorStyle(style);

	rw->Render();
	rwi->Initialize();
	rwi->Start();

	return EXIT_SUCCESS;
}