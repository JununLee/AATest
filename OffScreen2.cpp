#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkContourFilter.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkPlane.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>


int main_2()
{
	vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	//读取STL文件以创建读源对象 
	const char* inputFilename = "C:\\Users\\qiang.shen\\Desktop\\IronData\\XZW.stl";
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(inputFilename);
	//手动触发流水线，否则reader里面没有值
	reader->Update();

	//获取读源对象的轮廓[Xmin,Xmax, Ymin, Ymax, Zmin, Zmax]
	double bounds[6];
	reader->GetOutput()->GetBounds(bounds);
	std::cout << "Bounds: "
		<< bounds[0] << ", " << bounds[1] << " "
		<< bounds[2] << ", " << bounds[3] << " "
		<< bounds[4] << ", " << bounds[5] << std::endl;

	//创建一个平面程序源对象
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	//平面对象是由点和法向量定义的，这里设置平面经过的源对象的中心点坐标
	plane->SetOrigin((bounds[1] + bounds[0]) / 2.0,
		(bounds[3] + bounds[2]) / 2.0,
		(bounds[5] + bounds[4]) / 2.0);
	//设置平面对象的法向量;
	plane->SetNormal(0, 0, 1);

	//创建一个double型的标量数组
	vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();
	//获取源对象的点数
	int numberOfPoints = reader->GetOutput()->GetNumberOfPoints();
	//分配numberOfPoints属性数据的空间
	scalars->SetNumberOfTuples(numberOfPoints);
	//获取源对象的点集
	vtkSmartPointer<vtkPoints> pts = reader->GetOutput()->GetPoints();
	std::vector<double> aa;
	for (int i = 0; i < numberOfPoints; ++i)
	{
		double point[3];
		//获取点的坐标
		pts->GetPoint(i, point);
		//EvaluateFunction 评估点的平面方程(本质是求点point与平面中心点连接的向量在平面法向量上的投影)
		double evaValue = plane->EvaluateFunction(point);
		aa.push_back(evaValue);
		scalars->SetTuple1(i, evaValue);
	}
	reader->GetOutput()->GetPointData()->SetScalars(scalars);
	auto range = reader->GetOutput()->GetPointData()->GetScalars()->GetRange();

	// Create cutter
	vtkSmartPointer<vtkContourFilter> cutter = vtkSmartPointer<vtkContourFilter>::New();
	cutter->SetInputConnection(reader->GetOutputPort());
	cutter->ComputeScalarsOff();
	cutter->ComputeNormalsOff();
	int numberOfCuts = 10;
	//生成给定的标量值
	cutter->GenerateValues(numberOfCuts, 0.99 * range[0], 0.99 * range[1]);//设置需要抽取的标量值

	vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterMapper->SetInputConnection(cutter->GetOutputPort());
	//避免点的属性值被当做颜色来处理了
	cutterMapper->ScalarVisibilityOff();

	// Create cut actor
	vtkSmartPointer<vtkActor> cutterActor = vtkSmartPointer<vtkActor>::New();
	cutterActor->GetProperty()->SetColor(colors->GetColor3d("Banana").GetData());
	cutterActor->GetProperty()->SetLineWidth(2);
	cutterActor->SetMapper(cutterMapper);

	// Create model actor
	vtkSmartPointer<vtkPolyDataMapper> modelMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	modelMapper->SetInputConnection(reader->GetOutputPort());
	modelMapper->ScalarVisibilityOff();

	vtkSmartPointer<vtkActor> modelActor = vtkSmartPointer<vtkActor>::New();
	modelActor->GetProperty()->SetColor(colors->GetColor3d("Flesh").GetData());
	modelActor->SetMapper(modelMapper);

	// Create renderers and add actors of plane and model
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(cutterActor);
	renderer->AddActor(modelActor);

	// Add renderer to renderwindow and render
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(600, 600);

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(renderWindow);

	renderer->SetBackground(colors->GetColor3d("Burlywood").GetData());
	renderer->GetActiveCamera()->SetPosition(0, -1, 0);
	renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
	renderer->GetActiveCamera()->Azimuth(30);
	renderer->GetActiveCamera()->Elevation(30);

	renderer->ResetCamera();
	renderWindow->Render();

	interactor->Start();

	return EXIT_SUCCESS;
}