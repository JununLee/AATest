#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkWindowToImageFilter.h>
#include <vtkContourFilter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImageMapper.h>
#include <vtkImageActor.h>
#include <vtkCubeSource.h>

int main_o() {
	vtkSmartPointer<vtkPolyData> data3d;

	vtkSmartPointer<vtkSphereSource> source = vtkSmartPointer<vtkSphereSource>::New();
	source->SetCenter(0.0, 0.0, 5.0);
	source->SetRadius(2.0);
	source->SetPhiResolution(50.);
	source->SetThetaResolution(50.);
	source->Update();

	vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
	cubeSource->Update();

	//data3d = source->GetOutput();
	data3d = cubeSource->GetOutput();

	double bounds_data[6], center_data[3];
	data3d->GetBounds(bounds_data);
	data3d->GetCenter(center_data);

	vtkSmartPointer<vtkPolyDataMapper> mapper_data = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_data->SetInputData(data3d);

	vtkSmartPointer<vtkActor> actor_data = vtkSmartPointer<vtkActor>::New();
	actor_data->SetMapper(mapper_data);
	actor_data->GetProperty()->SetColor(1, 0, 0);
	//actor_data->GetProperty()->SetLighting(false);
	actor_data->RotateX(30);
	//actor_data->RotateY(30);
	//actor_data->RotateZ(30);

	vtkSmartPointer<vtkRenderer> tmp_rend = vtkSmartPointer<vtkRenderer>::New();
	tmp_rend->SetBackground(0, 0, 1);
	//tmp_rend->SetViewport(0, 0, 1, 1);

	tmp_rend->AddActor(actor_data);
	tmp_rend->ResetCamera();
	//开启平行光投影
	tmp_rend->GetActiveCamera()->SetParallelProjection(1);
	//tmp_rend->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	//tmp_rend->GetActiveCamera()->SetPosition(0, 0, 20);

	vtkSmartPointer<vtkRenderWindow> tmp_rW = vtkSmartPointer<vtkRenderWindow>::New();
	tmp_rW->SetOffScreenRendering(1);
	tmp_rW->SetSize(800, 800);
	tmp_rW->AddRenderer(tmp_rend);

	tmp_rW->Render();

	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
	//获取离屏渲染的窗口图像
	windowToImageFilter->SetInput(tmp_rW);

#if VTK_MAJOR_VERSION >= 8 || VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 90
	windowToImageFilter->SetScale(1); // image quality
#else
	//设置当前渲染窗口的方法倍数
	windowToImageFilter->SetMagnification(2); //image quality
#endif
	//手动触发流水线
	windowToImageFilter->Update();

	actor_data->GetProperty()->SetColor(1, 1, 0);
	tmp_rW->Render();
	vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter1 = vtkSmartPointer<vtkWindowToImageFilter>::New();
	//获取离屏渲染的窗口图像
	windowToImageFilter1->SetInput(tmp_rW);

//#if VTK_MAJOR_VERSION >= 8 || VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 90
//	windowToImageFilter1->SetScale(1); // image quality
//#else
//	//设置当前渲染窗口的方法倍数
//	windowToImageFilter1->SetMagnification(2); //image quality
//#endif
//	//手动触发流水线
	windowToImageFilter1->Update();



	// 提取图像黑白交界的轮廓
	vtkSmartPointer<vtkContourFilter> ContFilter = vtkSmartPointer<vtkContourFilter>::New();
	ContFilter->SetInputConnection(windowToImageFilter->GetOutputPort());
	//提取标量值为255的轮廓线,0表示第1条轮廓
	ContFilter->SetValue(0, 200);
	ContFilter->SetValue(1, 100);
	ContFilter->Update();

	//获取轮廓数据集——POLYDATA
	vtkSmartPointer<vtkPolyData> contour = ContFilter->GetOutput();


	vtkSmartPointer<vtkImageActor> imgActor = vtkSmartPointer<vtkImageActor>::New();
	imgActor->SetInputData(windowToImageFilter1->GetOutput()); 
	//imgActor->SetInputData(ContFilter->GetOutput());
	vtkImageData* imagedata = windowToImageFilter->GetOutput();

	unsigned char* pixels =(unsigned char*) imagedata->GetScalarPointer(400,400,0);

	// 重新设置程序源对象的颜色
	actor_data->GetProperty()->SetColor(0.9, 0.9, 0.8);

	// Create a mapper and actor of the silhouette
	vtkSmartPointer<vtkPolyDataMapper> mapper_contour = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper_contour->SetInputData(contour);
	//mapper_contour->SetInputConnection(windowToImageFilter->GetOutputPort());

	vtkSmartPointer<vtkActor> actor_contour = vtkSmartPointer<vtkActor>::New();
	actor_contour->SetMapper(mapper_contour);
	actor_contour->GetProperty()->SetLineWidth(2.);

	// 2 renderers and a render window
	vtkSmartPointer<vtkRenderer> renderer1 = vtkSmartPointer<vtkRenderer>::New();
	//renderer1->AddActor(actor_data);           
	renderer1->AddActor(imgActor);

	vtkSmartPointer<vtkRenderer> renderer2 = vtkSmartPointer<vtkRenderer>::New();
	renderer2->AddActor(actor_contour);

	vtkSmartPointer<vtkRenderWindow> renderwindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderwindow->SetSize(1600, 800);

	renderwindow->AddRenderer(renderer1);
	renderer1->SetViewport(0., 0., 0.5, 1.);
	renderer1->SetBackground(0, 1, 0);

	renderwindow->AddRenderer(renderer2);
	renderer2->SetBackground(0, 1, 1);
	renderer2->SetViewport(0.5, 0., 1., 1.);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	 
	iren->SetRenderWindow(renderwindow);
	iren->SetInteractorStyle(style);


	renderwindow->Render();
	iren->Start();



	return EXIT_SUCCESS;
}