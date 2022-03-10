#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkROIStencilSource.h>
#include <vtkImageData.h>
#include <vtkImageStencil.h>
#include <vtkImageToImageStencil.h>
#include <vtkExtractVOI.h>
#include <vector>

void CreateMask(vtkImageData* image)
{ 
	image->SetDimensions(512, 512, 415);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

	for (unsigned int x = 0; x < 512; x++)
	{
		for (unsigned int y = 0; y < 512; y++)
		{
			for (unsigned int z = 0; z < 415; z++) {
				unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, z));
				if (x < 512 / 2)
				{
					pixel[0] = 0;
				}
				else
				{
					pixel[0] = 255;
				}
			}
		}
	}

	image->Modified();
}

int main_V() {

	vtkSmartPointer<vtkDICOMImageReader> volumeReader = vtkSmartPointer<vtkDICOMImageReader>::New();
	volumeReader->SetDirectoryName("E:\\Data\\lxm");
	volumeReader->Update();

	vtkImageData* imageData = volumeReader->GetOutput();
	//double* origin = imageData->GetOrigin();
	//double* spaciing = imageData->GetSpacing();
	//int* dimension = imageData->GetDimensions();

	//vtkSmartPointer<vtkImageData> mask = vtkSmartPointer<vtkImageData>::New();
	//CreateMask(mask);

	vtkSmartPointer<vtkROIStencilSource> roi = vtkSmartPointer<vtkROIStencilSource>::New();   
	roi->SetBounds(0, 512, 0, 512, 0, 415);
	roi->Update();

	//vtkSmartPointer<vtkImageToImageStencil> imageToImageStencil = vtkSmartPointer<vtkImageToImageStencil>::New();
	//imageToImageStencil->SetInputData(mask);
	//imageToImageStencil->ThresholdByUpper(122);

	//vtkSmartPointer<vtkExtractVOI> voi = vtkSmartPointer<vtkExtractVOI>::New();
	//voi->SetInputConnection(volumeReader->GetOutputPort());
	//voi->SetVOI(0, 512, 40, 512, 0, 415);
	//voi->Update();

	vtkSmartPointer<vtkImageStencil> stencil = vtkSmartPointer<vtkImageStencil>::New(); 
	stencil->SetInputConnection(2, roi->GetOutputPort());
	stencil->SetInputData(imageData);
	//ellipseStencil->ReverseStencilOn();
	double dRange[2] = { .0 };
	//imageData->GetScalarRange(dRange);
	stencil->SetBackgroundValue(-1000);
	stencil->Update(); 

	vtkSmartPointer<vtkSmartVolumeMapper> mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	//mapper->SetInputData(volumeReader->GetOutput());
	mapper->SetInputData(stencil->GetOutput());
	//mapper->SetInputData(voi->GetOutput());

	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(mapper);

	vtkSmartPointer<vtkPiecewiseFunction> opacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacity->AddPoint(-1000, 0);
	opacity->AddPoint(152, 0); 
	opacity->AddPoint(278, 0.190476);
	opacity->AddPoint(952, 0.2); 

	vtkSmartPointer<vtkPiecewiseFunction> gradient = vtkSmartPointer<vtkPiecewiseFunction>::New();
	gradient->AddPoint(0, 1);
	gradient->AddPoint(985, 1);

	vtkSmartPointer<vtkColorTransferFunction> color = vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(-1000, 0.3, 0.3, 1);
	color->AddRGBPoint(-488, 0.3, 1, 0.3); 
	color->AddRGBPoint(463, 1, 0, 0);
	color->AddRGBPoint(659, 1, 0.912535, 0.0374849);
	color->AddRGBPoint(952, 1, 0.3, 0.3);

	volume->GetProperty()->SetInterpolationTypeToLinear();
	volume->GetProperty()->SetShade(1);
	volume->GetProperty()->SetDiffuse(1);
	volume->GetProperty()->SetAmbient(0.2);
	volume->GetProperty()->SetSpecular(0);
	volume->GetProperty()->SetSpecularPower(1);

	volume->GetProperty()->SetScalarOpacity(opacity);
	volume->GetProperty()->SetGradientOpacity(gradient);
	volume->GetProperty()->SetColor(color); 

	vtkPiecewiseFunction *trans = volume->GetProperty()->GetScalarOpacity(); 
	
	//int size = trans->GetSize(); 
	//std::vector<std::vector<double>> points;
	//points.resize(size);
	//for (int i = 0; i < size; i++)
	//{ 
	//	double val[4];
	//	trans->GetNodeValue(i, val);
	//	val[0] += -100;
	//	points[i] = { val[0],val[1],val[2],val[3] };
	//}
	//trans->RemoveAllPoints();
	//for (int i = 0; i < size; i++)
	//{
	//	double val[4] = { points[i][0],points[i][1], points[i][2], points[i][3] };
	//	trans->AddPoint(val[0], val[1]);
	//}

	vtkSmartPointer<vtkRenderer> render = vtkSmartPointer<vtkRenderer>::New();
	render->AddActor(volume);

	vtkSmartPointer<vtkRenderWindow> window = vtkSmartPointer<vtkRenderWindow>::New();
	window->AddRenderer(render);
	window->SetSize(800,800);

	vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(window);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);

	//interactor->SetDesiredUpdateRate(0.0001);

	interactor->Render();
	render->ResetCamera();
	interactor->Start();

	return 0;
}