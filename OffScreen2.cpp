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

	//��ȡSTL�ļ��Դ�����Դ���� 
	const char* inputFilename = "C:\\Users\\qiang.shen\\Desktop\\IronData\\XZW.stl";
	vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(inputFilename);
	//�ֶ�������ˮ�ߣ�����reader����û��ֵ
	reader->Update();

	//��ȡ��Դ���������[Xmin,Xmax, Ymin, Ymax, Zmin, Zmax]
	double bounds[6];
	reader->GetOutput()->GetBounds(bounds);
	std::cout << "Bounds: "
		<< bounds[0] << ", " << bounds[1] << " "
		<< bounds[2] << ", " << bounds[3] << " "
		<< bounds[4] << ", " << bounds[5] << std::endl;

	//����һ��ƽ�����Դ����
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	//ƽ��������ɵ�ͷ���������ģ���������ƽ�澭����Դ��������ĵ�����
	plane->SetOrigin((bounds[1] + bounds[0]) / 2.0,
		(bounds[3] + bounds[2]) / 2.0,
		(bounds[5] + bounds[4]) / 2.0);
	//����ƽ�����ķ�����;
	plane->SetNormal(0, 0, 1);

	//����һ��double�͵ı�������
	vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();
	//��ȡԴ����ĵ���
	int numberOfPoints = reader->GetOutput()->GetNumberOfPoints();
	//����numberOfPoints�������ݵĿռ�
	scalars->SetNumberOfTuples(numberOfPoints);
	//��ȡԴ����ĵ㼯
	vtkSmartPointer<vtkPoints> pts = reader->GetOutput()->GetPoints();
	std::vector<double> aa;
	for (int i = 0; i < numberOfPoints; ++i)
	{
		double point[3];
		//��ȡ�������
		pts->GetPoint(i, point);
		//EvaluateFunction �������ƽ�淽��(���������point��ƽ�����ĵ����ӵ�������ƽ�淨�����ϵ�ͶӰ)
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
	//���ɸ����ı���ֵ
	cutter->GenerateValues(numberOfCuts, 0.99 * range[0], 0.99 * range[1]);//������Ҫ��ȡ�ı���ֵ

	vtkSmartPointer<vtkPolyDataMapper> cutterMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cutterMapper->SetInputConnection(cutter->GetOutputPort());
	//����������ֵ��������ɫ��������
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