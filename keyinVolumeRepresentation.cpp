
#include "keyinVolumeRepresentation.h"
#include <vtkPointHandleRepresentation2D.h>
#include <vtkAxisActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkLineSource.h>
#include <vtkProperty2D.h>
#include <vtkCoordinate.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkBox.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkTextProperty.h>
#include <vtkWindow.h>
#include <vtkPNGReader.h>
#include <vtkRenderWindow.h>
#include <vtkSmoothPolyDataFilter.h>
#include "keyinVolumeCalculate.h"

vtkStandardNewMacro(keyinVolumeRepresentation);

//----------------------------------------------------------------------
keyinVolumeRepresentation::keyinVolumeRepresentation()
{
	this->VolumeProperty = vtkProperty::New();
	this->VolumeMapper = vtkPolyDataMapper::New();
	this->VolumeActor = vtkActor::New();
	this->VolumeActor->SetMapper(this->VolumeMapper);
	this->VolumeActor->SetProperty(this->VolumeProperty);
	
	this->LabelTextProperty = vtkTextProperty::New();
	this->LabelTextProperty->SetBold(0);
	this->LabelTextProperty->SetItalic(0);
	this->LabelTextProperty->SetShadow(1);
	this->LabelTextProperty->SetFontFamilyToArial();
	this->LabelTextProperty->SetJustificationToCentered();
	this->LabelTextProperty->SetVerticalJustificationToCentered();

	this->LabelMapper = vtkTextMapper::New();
	this->LabelActor = vtkActor2D::New();
	this->LabelMapper->SetTextProperty(this->LabelTextProperty);
	this->LabelActor->SetMapper(this->LabelMapper);

	
	this->imageMapper = vtkImageMapper::New();
	this->imageMapper->SetColorWindow(256);
	this->imageMapper->SetColorLevel(128);
	this->imageActor = vtkActor2D::New();
	this->imageActor->SetMapper(imageMapper);

}

//----------------------------------------------------------------------
keyinVolumeRepresentation::~keyinVolumeRepresentation()
{
	this->VolumeActor->Delete();
	this->VolumeMapper->Delete();
	this->VolumeProperty->Delete();
	this->LabelActor->Delete();
	this->LabelMapper->Delete();
	this->LabelTextProperty->Delete();
	this->imageActor->Delete();
	this->imageMapper->Delete();
}

void keyinVolumeRepresentation::volumeCalculate(vtkImageData * input, int * seed, int lower, int upper)
{
	vtkMarchingCubes* mc = keyinVolumeCalculate::volumeMarchingCube(input, seed, lower, upper, Volume);
	//vtkSmartPointer<vtkSmoothPolyDataFilter> smootherPoly = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	//smootherPoly->SetRelaxationFactor(0.33);
	//smootherPoly->SetFeatureAngle(60);
	//smootherPoly->SetConvergence(0);
	//smootherPoly->SetInputConnection(mc->GetOutputPort());
	//smootherPoly->SetNumberOfIterations(10);
	//smootherPoly->FeatureEdgeSmoothingOff();
	//smootherPoly->BoundarySmoothingOff();
	//smootherPoly->ReleaseDataFlagOn();
	//smootherPoly->Update();
	//this->VolumeMapper->SetInputConnection(smootherPoly->GetOutputPort());

	this->VolumeMapper->SetInputConnection(mc->GetOutputPort());

	this->Renderer->AddActor(VolumeActor);
	this->Renderer->AddActor(imageActor);
	this->Renderer->AddActor(LabelActor);
	this->Renderer->GetRenderWindow()->Render();
}


int keyinVolumeRepresentation::ComputeInteractionState(int vtkNotUsed(X), int vtkNotUsed(Y), int vtkNotUsed(modify))
{

	return this->InteractionState;
}

int keyinVolumeRepresentation::ComputeComplexInteractionState(vtkRenderWindowInteractor *, vtkAbstractWidget *, unsigned long, void *, int)
{

	return this->InteractionState;
}

void keyinVolumeRepresentation::SetTextBackImageFile(const char * textImageFile)
{
	vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
	reader->SetFileName(textImageFile);
	reader->Update();
	reader->GetOutput()->GetDimensions(this->imageSize);
	this->imageMapper->SetInputData(reader->GetOutput());
}


//----------------------------------------------------------------------
void keyinVolumeRepresentation::BuildRepresentation()
{
	//if (this->GetMTime() > this->BuildTime || this->VolumeActor->GetMTime() > this->BuildTime || this->LabelTextProperty->GetMTime() > this->BuildTime  || (this->Renderer && this->Renderer->GetVTKWindow() && this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))
	{
		this->Superclass::BuildRepresentation();


		char string[512];
		snprintf(string, sizeof(string), this->LabelFormat, this->Volume);
		this->LabelMapper->SetInput(string);

		double* p = VolumeActor->GetCenter();
		this->Renderer->SetWorldPoint(p[0], p[1], p[2], 1);
		this->Renderer->WorldToDisplay();
		this->Renderer->GetDisplayPoint(p);
		this->LabelActor->SetPosition(p[0] + (this->imageSize[0]) / 2, p[1]);

		this->imageActor->SetPosition(p[0], p[1] - this->imageSize[1] / 2);
		
		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
	this->VolumeActor->ReleaseGraphicsResources(w);
	this->LabelActor->ReleaseGraphicsResources(w);
	this->imageActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int keyinVolumeRepresentation::RenderOverlay(vtkViewport *v)
{
	this->BuildRepresentation();

	return 0;
}

//----------------------------------------------------------------------
int keyinVolumeRepresentation::RenderOpaqueGeometry(vtkViewport *v)
{
	this->BuildRepresentation();
	return 0;
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);
}
void keyinVolumeRepresentation::GetPoint1WorldPosition(double pos[3])
{

}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::GetPoint2WorldPosition(double pos[3])
{
	
}

//----------------------------------------------------------------------
double* keyinVolumeRepresentation::GetPoint1WorldPosition()
{
	return 0;
}

//----------------------------------------------------------------------
double* keyinVolumeRepresentation::GetPoint2WorldPosition()
{
	return 0;
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::SetPoint1DisplayPosition(double x[3])
{
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::SetPoint2DisplayPosition(double x[3])
{
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::SetPoint1WorldPosition(double x[3])
{
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::SetPoint2WorldPosition(double x[3])
{
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::GetPoint1DisplayPosition(double pos[3])
{
}

//----------------------------------------------------------------------
void keyinVolumeRepresentation::GetPoint2DisplayPosition(double pos[3])
{
}