#ifndef KEYINVOLUMECALCULATE_H
#define KEYINVOLUMECALCULATE_H

#include <vtkImageData.h>
#include <queue>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>

class keyinVolumeCalculate
{
public:
	keyinVolumeCalculate();
	~keyinVolumeCalculate();

	static void floodFill(vtkImageData *input, vtkImageData* output, int dims[3], int x, int y, int z, int lower, int upper, int& volume);
	static void floodFill(vtkImageData *input, vtkImageData* output, std::queue<std::vector<int>> q, int dims[3], int lower, int upper, int& volume);
	static void floodFill(vtkImageData* input, vtkImageData* output, int seed[3], int lower, int upper, double& volume);
	static vtkMarchingCubes* volumeMarchingCube(vtkImageData* input, int* seed, int lower, int upper, double& volume);

private:

};


#endif // !KEYINVOLUMECALCULATE_H

