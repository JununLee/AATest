#include "keyinVolumeCalculate.h"

keyinVolumeCalculate::keyinVolumeCalculate()
{
}

keyinVolumeCalculate::~keyinVolumeCalculate()
{
}

void keyinVolumeCalculate::floodFill(vtkImageData *input, vtkImageData* output, int dims[3], int x, int y, int z, int lower, int upper, int& volume) {

	if (x >= dims[0] || x < 0 || y >= dims[1] || y < 0 || z >= dims[2] || z < 0) {
		return;
	}
	short * op = (short *)(output->GetScalarPointer(x, y, z));
	if (*op >= 1) {
		return;
	}
	short * ip = (short *)(input->GetScalarPointer(x, y, z));
	if (*ip < lower || *ip>upper) {
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

void keyinVolumeCalculate::floodFill(vtkImageData *input, vtkImageData* output, std::queue<std::vector<int>> q, int dims[3], int lower, int upper, int& volume) {

	while (!q.empty())
	{
		std::vector<int> var = q.front();
		short * op = (short *)(output->GetScalarPointer(var[0], var[1], var[2]));
		short * ip = (short *)(input->GetScalarPointer(var[0], var[1], var[2]));
		q.pop();
		/*if (*op >= 1 || *ip<lower || *ip>upper) {
			continue;
		}
		*op = 1;*/
		if (*op >= 1) {
			continue;
		}
		if (*ip<lower || *ip>upper) {
			*op = *ip;
			continue;
		}
		*op = *ip;
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
			std::vector<int>  point = { var[0], var[1], var[2] - 1 };
			q.push(point);
		}
		if (var[2] + 1 < dims[2]) {
			std::vector<int>  point = { var[0], var[1], var[2] + 1 };
			q.push(point);
		}
	}
}

void keyinVolumeCalculate::floodFill(vtkImageData* input, vtkImageData* output, int seed[3], int lower, int upper, double& volume) {
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

vtkMarchingCubes* keyinVolumeCalculate::volumeMarchingCube(vtkImageData* input, int* seed, int lower, int upper, double& volume) {


	vtkSmartPointer<vtkImageData> output = vtkSmartPointer<vtkImageData>::New();

	floodFill(input, output, seed, lower, upper, volume);

	vtkMarchingCubes* mc = vtkMarchingCubes::New();
	mc->SetInputData(output);
	mc->SetValue(0, lower);
	mc->ComputeGradientsOn();
	mc->ComputeNormalsOn();
	mc->ComputeScalarsOff();

	return mc;
}