 
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkPLYReader.h>
#include <sstream>
#include <iostream>
#include <vtkInteractorStyleTrackballCamera.h>
#include <Windows.h> 
#include <vtkPropCollection.h>

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

vtkSmartPointer<vtkPoints> loadPLY(const std::string& file_name) 
{
	vtkSmartPointer<vtkPoints> sourcePoints = vtkSmartPointer<vtkPoints>::New();
	std::ifstream file(file_name);
	if (!file.is_open()) {
		cout << "cant open" << "\n";
	}
	std::string line;
	std::vector<std::string> v;
	int size;
	while (std::getline(file, line))
	{ 
		v.clear();
		SplitString(line, v, " ");
		if (v.size() > 2 && v[0] == "element"&&v[1] == "vertex") {
			cout << v[2] << "\n";
			size = std::stoi(v[2]);
			//cloud.points.resize(len);
		}
		if (v.size() > 0 && v[0] == "end_header") {
			for (int i = 0; i < size; i++)
			{
				v.clear();
				std::getline(file, line);
				SplitString(line, v, " "); 
				sourcePoints->InsertNextPoint(std::atof(v[0].c_str()), std::atof(v[1].c_str()), std::atof(v[2].c_str()));
			}
			break;
		}
	} 
	file.close();
	return sourcePoints;
}

int main_P()
{
	vtkSmartPointer<vtkPolyData> sourceTmp = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyData> targetTmp = vtkSmartPointer<vtkPolyData>::New();

	vtkSmartPointer<vtkPolyData> source = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPolyData> target = vtkSmartPointer<vtkPolyData>::New();

 
	//vtkSmartPointer<vtkPoints> sourcePoints = loadPLY("C:\\Users\\dell\\Desktop\\PCL\\monkey3.ply");
	vtkSmartPointer<vtkPoints> sourcePoints = loadPLY("C:\\Users\\dell\\Desktop\\PCL\\monkey.ply");
	vtkSmartPointer<vtkPoints> targetPoints = loadPLY("C:\\Users\\dell\\Desktop\\PCL\\monkey2.ply");

 
	sourceTmp->SetPoints(sourcePoints);
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter1 = vtkSmartPointer<vtkVertexGlyphFilter>::New(); 
	vertexFilter1->SetInputData(sourceTmp); 
	vertexFilter1->Update();
	source->ShallowCopy(vertexFilter1->GetOutput());
	 
	targetTmp->SetPoints(targetPoints);
	vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter2 = vtkSmartPointer<vtkVertexGlyphFilter>::New(); 
	vertexFilter2->SetInputData(targetTmp); 
	vertexFilter2->Update();
	target->ShallowCopy(vertexFilter2->GetOutput());
	 
	vtkSmartPointer<vtkTransform> pTransform = vtkSmartPointer<vtkTransform>::New();
	vtkMatrix4x4* mat = vtkMatrix4x4::New();
	//mat->SetElement(0, 0, 0.952759);
	//mat->SetElement(0, 1, 0.0861466);
	//mat->SetElement(0, 2, 0.291254);
	//mat->SetElement(0, 3, 1.13814);

	//mat->SetElement(1, 0, -0.0143186);
	//mat->SetElement(1, 1, 0.970607);
	//mat->SetElement(1, 2, -0.240245);
	//mat->SetElement(1, 3, 0.00630188);

	//mat->SetElement(2, 0, -0.30339);
	//mat->SetElement(2, 1, 0.224726);
	//mat->SetElement(2, 2, 0.925988);
	//mat->SetElement(2, 3, 0.0520905);

	mat->SetElement(0, 0, 0.967173);
	mat->SetElement(0, 1, 0.066146);
	mat->SetElement(0, 2, 0.245357);
	mat->SetElement(0, 3, 1.15212);

	mat->SetElement(1, 0, -0.000733048);
	mat->SetElement(1, 1, 0.966251);
	mat->SetElement(1, 2, -0.257602);
	mat->SetElement(1, 3, -0.0010359);

	mat->SetElement(2, 0, -0.254116);
	mat->SetElement(2, 1, 0.248966);
	mat->SetElement(2, 2, 0.93458);
	mat->SetElement(2, 3, 0.0078544);

	mat->SetElement(3, 0, 0);
	mat->SetElement(3, 1, 0);
	mat->SetElement(3, 2, 0);
	mat->SetElement(3, 3, 1);

	pTransform->SetMatrix(mat);
	vtkSmartPointer<vtkTransformPolyDataFilter> pTransformPolyDataFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	pTransformPolyDataFilter->SetInputData(source);
	pTransformPolyDataFilter->SetTransform(pTransform);
	pTransformPolyDataFilter->Update(); 
	sourceTmp->ShallowCopy(pTransformPolyDataFilter->GetOutput());

	std::cout << "tickcout--start" << GetTickCount() << "\n";
	// Setup ICP transform
	vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
	icp->SetSource(sourceTmp);
	icp->SetTarget(target);

	icp->GetLandmarkTransform()->SetModeToRigidBody(); 
	icp->SetMaximumNumberOfIterations(20);
	std::cout << "pointsNum" << sourceTmp->GetNumberOfPoints() << "\n";
	icp->SetMaximumNumberOfLandmarks(20000);
	//icp->StartByMatchingCentroidsOn();

	icp->Modified();
	icp->Update(); 
	std::cout << "tickcout--end" << GetTickCount() << "\n";

	// Get the resulting transformation matrix (this matrix takes the source points to the target points)
	vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
	//vtkMatrix4x4::Multiply4x4(mat, m, m);
	std::cout << "The resulting matrix is: " << *m << std::endl;

	// Transform the source points by the ICP solution
	vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

	icpTransformFilter->SetInputData(sourceTmp);
	icpTransformFilter->SetTransform(icp);
	icpTransformFilter->Update();


	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> sourceMapper = vtkSmartPointer<vtkPolyDataMapper>::New(); 
	sourceMapper->SetInputData(source);
	vtkSmartPointer<vtkActor> sourceActor = vtkSmartPointer<vtkActor>::New();
	sourceActor->SetMapper(sourceMapper);
	sourceActor->GetProperty()->SetColor(1, 0, 0);
	sourceActor->GetProperty()->SetPointSize(3);

	vtkSmartPointer<vtkPolyDataMapper> targetMapper = vtkSmartPointer<vtkPolyDataMapper>::New(); 
	targetMapper->SetInputData(target); 
	vtkSmartPointer<vtkActor> targetActor = vtkSmartPointer<vtkActor>::New();
	targetActor->SetMapper(targetMapper);
	targetActor->GetProperty()->SetColor(0, 1, 0);
	targetActor->GetProperty()->SetPointSize(3);

	vtkSmartPointer<vtkPolyDataMapper> solutionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	//solutionMapper->SetInputConnection(icpTransformFilter->GetOutputPort()); 
	solutionMapper->SetInputData(sourceTmp);
	vtkSmartPointer<vtkActor> solutionActor = vtkSmartPointer<vtkActor>::New();
	solutionActor->SetMapper(solutionMapper);
	solutionActor->GetProperty()->SetColor(0, 0, 1);
	solutionActor->GetProperty()->SetPointSize(3);

	// Create a renderer, render window, and interactor
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(800, 800);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);

	vtkPropCollection* props = renderer->GetViewProps(); 
	//props->InsertItem(0, sourceActor);

	props->AddItem(sourceActor);

	// Add the actor to the scene
	//renderer->AddActor(sourceActor);
	renderer->AddActor(targetActor);
	renderer->AddActor(solutionActor);
	renderer->SetBackground(.3, .6, .3); // Background color green

	// Render and interact
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}
