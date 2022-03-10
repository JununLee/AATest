#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkVectorNorm.h>
#include <vtkDataSetMapper.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <queue>
#include <vtkDelaunay3D.h>
#include <vtkPNGReader.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkSphereSource.h>
#include "keyinVolumeCalculate.h"


void floodFill(vtkImageData *input, vtkImageData* output,int dims[3], int x, int y, int z, int lower, int upper, int& volume) {

	if (x >= dims[0] || x < 0 || y >= dims[1] || y < 0 || z >= dims[2] || z < 0) {
		return;
	}
	short * op = (short *)(output->GetScalarPointer(x, y, z));
	if (*op >= 1) {
		return;
	}
	short * ip = (short *)(input->GetScalarPointer(x, y, z));
	if (*ip < lower||*ip>upper) {
		return;
	}

	*op = 1;
	volume++;
	
	floodFill(input, output, dims, x - 1, y, z, lower, upper, volume);
	floodFill(input, output, dims, x + 1, y, z, lower, upper, volume);
	floodFill(input, output, dims, x, y - 1, z, lower, upper, volume);
	floodFill(input, output, dims, x, y + 1, z, lower, upper, volume);
	floodFill(input, output, dims, x, y, z - 1, lower, upper, volume);
	floodFill(input, output, dims, x, y, z + 1, lower, upper, volume);
}

void floodFill(vtkImageData *input, vtkImageData* output, std::queue<std::vector<int>> q,int dims[3], int lower, int upper, int& volume) {

	while (!q.empty())
	{
		std::vector<int> var = q.front();
		short * op = (short *)(output->GetScalarPointer(var[0], var[1], var[2]));
		short * ip = (short *)(input->GetScalarPointer(var[0], var[1], var[2]));
		q.pop();
		if (*op >= 1 || *ip<lower || *ip>upper) {
			continue;
		}
		*op = 1;
		volume++;
		if (var[0] - 1 >= 0) {
			std::vector<int>  point = { var[0] - 1, var[1], var[2] };
			q.push(point);
		}
		if (var[0] + 1 < dims[0]) {
			std::vector<int>  point = { var[0] + 1, var[1], var[2] };
			q.push(point);
		}
		if (var[1] - 1 >= 0) {
			std::vector<int>  point = { var[0], var[1] - 1, var[2] };
			q.push(point);
		}
		if (var[1] + 1 < dims[1]) {
			std::vector<int>  point = { var[0], var[1] + 1, var[2] };
			q.push(point);
		}
		if (var[2] - 1 >= 0) {
			std::vector<int>  point = { var[0], var[1], var[2] -1 };
			q.push(point);
		}
		if (var[2] + 1 < dims[2]) {
			std::vector<int>  point = { var[0], var[1], var[2] + 1 };
			q.push(point);
		}
	}
}

void floodFill(vtkImageData* input,vtkImageData* output, int seed[3],int lower,int upper,double& volume) {
	int dims[3];
	input->GetDimensions(dims);
	double spaceing[3];
	input->GetSpacing(spaceing);
	double origin[3];
	input->GetOrigin(origin);

	vtkSmartPointer<vtkInformation> info = vtkSmartPointer<vtkInformation>::New();
	output->SetDimensions(dims);
	output->SetSpacing(spaceing);
	output->SetOrigin(origin);
	output->SetScalarType(VTK_SHORT, info);
	output->SetNumberOfScalarComponents(1, info);
	output->AllocateScalars(info);

	int num = 0;
	//floodFill(input, output, dims, seed[0], seed[1], seed[2], lower, upper, num);

	std::queue<std::vector<int>> q;
	std::vector<int>p;
	p.push_back(seed[0]);
	p.push_back(seed[1]);
	p.push_back(seed[2]);


	q.push(p);
	floodFill(input, output, q, dims, lower, upper, num);

	volume = num * spaceing[0] * spaceing[1] * spaceing[2];
}

vtkActor* volumeWidget(vtkImageData* input,int* seed,int lower,int upper, double& volume ) {


	vtkSmartPointer<vtkImageData> output = vtkSmartPointer<vtkImageData>::New();

	floodFill(input, output, seed, lower, upper,volume);

	vtkSmartPointer<vtkMarchingCubes> mc = vtkSmartPointer<vtkMarchingCubes>::New();
	mc->SetInputData(output);
	mc->SetValue(0, 1);
	mc->ComputeGradientsOn();
	mc->ComputeNormalsOn();
	mc->ComputeScalarsOff();

	vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cubeMapper->SetInputConnection(mc->GetOutputPort());

	vtkActor *cubeActor = vtkActor::New();
	cubeActor->SetMapper(cubeMapper);
	cubeActor->GetProperty()->SetColor(1, 1, 1);
	
	return cubeActor;
}


int main_M() {


	vtkSmartPointer<vtkDICOMImageReader> volumeReader = vtkSmartPointer<vtkDICOMImageReader>::New();
	volumeReader->SetDirectoryName("G:\\test\\DCM_liaoximin");
	volumeReader->Update();
	vtkImageData* input = volumeReader->GetOutput();

	double volume = 0;
	int seed[3] = { 275,181,318 };
	//vtkActor* cubeActor = volumeWidget(input, seed,200, 4000, volume);
	vtkMarchingCubes* mc = keyinVolumeCalculate::volumeMarchingCube(input, seed, 200, 4000, volume);

	vtkSmartPointer<vtkPolyDataMapper> cubeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	cubeMapper->SetInputConnection(mc->GetOutputPort());

	vtkActor *cubeActor = vtkActor::New();
	cubeActor->SetMapper(cubeMapper);

	int imageSize[3];
	vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
	reader->SetFileName("C:\\Users\\Administrator\\Desktop\\myactor.png");
	reader->Update();
	reader->GetOutput()->GetDimensions(imageSize);
	vtkImageMapper* imageMapper = vtkImageMapper::New();
	imageMapper->SetInputData(reader->GetOutput());
	imageMapper->SetColorWindow(256);
	imageMapper->SetColorLevel(128);
	vtkActor2D* imageActor = vtkActor2D::New();
	imageActor->SetMapper(imageMapper);
	double* p = cubeActor->GetCenter();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	iren->SetInteractorStyle(style);


	renderer->AddActor(cubeActor);
	renderer->AddActor2D(imageActor);
	renderer->ResetCamera();
	renderer->SetBackground(0, 0, 0);

	renWin->SetSize(800, 800);
	renWin->Render();
	double p2[3];
	renderer->SetWorldPoint(p[0], p[1], p[2], 1);
	renderer->WorldToDisplay();
	renderer->GetDisplayPoint(p2);
	imageActor->SetPosition(p2);

	iren->Start();
	return 0;
}