
#include "keyinDistanceRepresentation.h"
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

vtkStandardNewMacro(keyinDistanceRepresentation);

//----------------------------------------------------------------------
keyinDistanceRepresentation::keyinDistanceRepresentation()
{
	// By default, use one of these handles
	this->HandleRepresentation = vtkPointHandleRepresentation2D::New();

	this->Point1Representation = nullptr;
	this->Point2Representation = nullptr;
	InstantiateHandleRepresentation();
	this->AxisProperty = vtkProperty2D::New();
	this->AxisProperty->SetColor(0, 1, 0);

	this->AxisActor = keyinActor2D::New();
	this->AxisActor->GetPoint1Coordinate()->SetCoordinateSystemToWorld();
	this->AxisActor->GetPoint2Coordinate()->SetCoordinateSystemToWorld();
	this->AxisActor->SetNumberOfLabels(5);
	this->AxisActor->LabelVisibilityOff();
	this->AxisActor->AdjustLabelsOff();
	this->AxisActor->SetProperty(this->AxisProperty);
	this->AxisActor->SetTitle("Distance");
	this->AxisActor->GetTitleTextProperty()->SetBold(0);
	this->AxisActor->GetTitleTextProperty()->SetItalic(0);
	this->AxisActor->GetTitleTextProperty()->SetShadow(1);
	this->AxisActor->GetTitleTextProperty()->SetFontFamilyToArial();

	this->Distance = 0.0;
}

//----------------------------------------------------------------------
keyinDistanceRepresentation::~keyinDistanceRepresentation()
{
	if (this->Point1Representation)
	{
		this->Point1Representation->Delete();
	}
	if (this->Point2Representation)
	{
		this->Point2Representation->Delete();
	}
	this->AxisProperty->Delete();
	this->AxisActor->Delete();
}

void keyinDistanceRepresentation::InstantiateHandleRepresentation()
{
	if (!this->Point1Representation)
	{
		this->Point1Representation = keyinHandleRepresentation::New();
	}

	if (!this->Point2Representation)
	{
		this->Point2Representation = keyinHandleRepresentation::New();
	}
}
int keyinDistanceRepresentation::ComputeInteractionState(int vtkNotUsed(X), int vtkNotUsed(Y), int vtkNotUsed(modify))
{
	if (this->Point1Representation == nullptr || this->Point2Representation == nullptr)
	{
		this->InteractionState = vtkDistanceRepresentation::Outside;
		return this->InteractionState;
	}

	int h1State = this->Point1Representation->GetInteractionState();
	int h2State = this->Point2Representation->GetInteractionState();
	if (h1State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkDistanceRepresentation::NearP1;
	}
	else if (h2State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkDistanceRepresentation::NearP2;
	}
	else
	{
		this->InteractionState = vtkDistanceRepresentation::Outside;
	}

	return this->InteractionState;
}

int keyinDistanceRepresentation::ComputeComplexInteractionState(vtkRenderWindowInteractor *,vtkAbstractWidget *,unsigned long, void *, int)
{
	if (this->Point1Representation == nullptr || this->Point2Representation == nullptr)
	{
		this->InteractionState = vtkDistanceRepresentation::Outside;
		return this->InteractionState;
	}

	int h1State = this->Point1Representation->GetInteractionState();
	int h2State = this->Point2Representation->GetInteractionState();
	if (h1State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkDistanceRepresentation::NearP1;
	}
	else if (h2State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkDistanceRepresentation::NearP2;
	}
	else
	{
		this->InteractionState = vtkDistanceRepresentation::Outside;
	}

	return this->InteractionState;
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::GetPoint1WorldPosition(double pos[3])
{
	this->Point1Representation->GetWorldPosition(pos);
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::GetPoint2WorldPosition(double pos[3])
{
	this->Point2Representation->GetWorldPosition(pos);
}

//----------------------------------------------------------------------
double* keyinDistanceRepresentation::GetPoint1WorldPosition()
{
	if (!this->Point1Representation)
	{
		static double temp[3] = { 0, 0, 0 };
		return temp;
	}
	return this->Point1Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
double* keyinDistanceRepresentation::GetPoint2WorldPosition()
{
	if (!this->Point2Representation)
	{
		static double temp[3] = { 0, 0, 0 };
		return temp;
	}
	return this->Point2Representation->GetWorldPosition();
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::SetPoint1DisplayPosition(double x[3])
{
	this->Point1Representation->SetDisplayPosition(x);
	double p[3];
	this->Point1Representation->GetWorldPosition(p);
	this->Point1Representation->SetWorldPosition(p);
	this->BuildRepresentation();
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::SetPoint2DisplayPosition(double x[3])
{
	this->Point2Representation->SetDisplayPosition(x);
	double p[3];
	this->Point2Representation->GetWorldPosition(p);
	this->Point2Representation->SetWorldPosition(p);
	this->BuildRepresentation();
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::SetPoint1WorldPosition(double x[3])
{
	if (this->Point1Representation)
	{
		this->Point1Representation->SetWorldPosition(x);
		this->BuildRepresentation();
	}
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::SetPoint2WorldPosition(double x[3])
{
	if (this->Point2Representation)
	{
		this->Point2Representation->SetWorldPosition(x);
		this->BuildRepresentation();
	}
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::GetPoint1DisplayPosition(double pos[3])
{
	this->Point1Representation->GetDisplayPosition(pos);
	pos[2] = 0.0;
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::GetPoint2DisplayPosition(double pos[3])
{
	this->Point2Representation->GetDisplayPosition(pos);
	pos[2] = 0.0;
}


keyinActor2D *keyinDistanceRepresentation::GetAxis()
{
	return this->AxisActor;
}

//----------------------------------------------------------------------
vtkProperty2D *keyinDistanceRepresentation::GetAxisProperty()
{
	return this->AxisActor->GetProperty();
}

void keyinDistanceRepresentation::SetTextBackImageFile(const char * textImageFile)
{
	this->AxisActor->SetTextBackImageFile(textImageFile);
}

void keyinDistanceRepresentation::SetPointImageFile(const char * pointImageFile)
{
	this->AxisActor->SetPointImageFile(pointImageFile);

}

void keyinDistanceRepresentation::SetHandleImageFile(const char * handleImageFile)
{
	this->Point1Representation->SetHandleImageFile(handleImageFile);
	this->Point2Representation->SetHandleImageFile(handleImageFile);
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::BuildRepresentation()
{
	if (this->GetMTime() > this->BuildTime ||
		this->AxisActor->GetMTime() > this->BuildTime ||
		this->AxisActor->GetTitleTextProperty()->GetMTime() > this->BuildTime ||
		this->Point1Representation->GetMTime() > this->BuildTime ||
		this->Point2Representation->GetMTime() > this->BuildTime ||
		(this->Renderer && this->Renderer->GetVTKWindow() &&
			this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))
	{
		this->Superclass::BuildRepresentation();

		// Okay, compute the distance and set the label
		double p1[3], p2[3];
		this->Point1Representation->GetWorldPosition(p1);
		this->Point2Representation->GetWorldPosition(p2);
		this->Distance = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));

		this->AxisActor->GetPoint1Coordinate()->SetValue(p1);
		this->AxisActor->GetPoint2Coordinate()->SetValue(p2);
		this->AxisActor->SetRulerMode(this->RulerMode);
		if (this->Scale != 0.0)
		{
			this->AxisActor->SetRulerDistance(this->RulerDistance / this->Scale);
		}
		this->AxisActor->SetNumberOfLabels(this->NumberOfRulerTicks);

		char string[512];
		snprintf(string, sizeof(string), this->LabelFormat, this->Distance * this->Scale);
		this->AxisActor->SetTitle(string);

		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
	this->AxisActor->ReleaseGraphicsResources(w);
}

//----------------------------------------------------------------------
int keyinDistanceRepresentation::RenderOverlay(vtkViewport *v)
{
	this->BuildRepresentation();
	//this->Point1Representation->com

	if (this->AxisActor->GetVisibility())
	{
		return this->AxisActor->RenderOverlay(v);
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------
int keyinDistanceRepresentation::RenderOpaqueGeometry(vtkViewport *v)
{
	this->BuildRepresentation();

	if (this->AxisActor->GetVisibility())
	{
		return this->AxisActor->RenderOpaqueGeometry(v);
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------
void keyinDistanceRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);
}
