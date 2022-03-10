
#include "keyinAngleRepresentation.h"
#include <vtkPointHandleRepresentation2D.h>
#include <vtkLeaderActor2D.h>
#include <vtkCoordinate.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkWindow.h>

vtkStandardNewMacro(keyinAngleRepresentation);


void keyinAngleRepresentation::SetTextBackImageFile(const char * textImageFile)
{
	this->Arc->SetTextBackImageFile(textImageFile);
}

void keyinAngleRepresentation::SetPointImageFile(const char * pointImageFile)
{
	this->Ray1->SetPointImageFile(pointImageFile);
	this->Ray2->SetPointImageFile(pointImageFile);
	this->Arc->SetPointImageFile(pointImageFile);
}

void keyinAngleRepresentation::SetHandleImageFile(const char * handleImageFile)
{
	this->Point1Representation->SetHandleImageFile(handleImageFile);
	this->CenterRepresentation->SetHandleImageFile(handleImageFile);
	this->Point2Representation->SetHandleImageFile(handleImageFile);
}

//----------------------------------------------------------------------
keyinAngleRepresentation::keyinAngleRepresentation()
{
	// By default, use one of these handles
	this->HandleRepresentation = vtkPointHandleRepresentation2D::New();
	this->Point1Representation = nullptr;
	this->CenterRepresentation = nullptr;
	this->Point2Representation = nullptr;
	InstantiateHandleRepresentation();

	this->Ray1 = keyinAngleActor::New();
	this->Ray1->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	this->Ray1->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
	//this->Ray1->SetArrowStyleToOpen();
	this->Ray1->SetArrowPlacementToPoint2();

	this->Ray2 = keyinAngleActor::New();
	this->Ray2->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	this->Ray2->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
	//this->Ray2->SetArrowStyleToOpen();
	this->Ray2->SetArrowPlacementToPoint2();

	this->Arc = keyinAngleActor::New();
	this->Arc->GetPositionCoordinate()->SetCoordinateSystemToWorld();
	this->Arc->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
	this->Arc->SetArrowPlacementToNone();
	//this->Arc->SetLabel("Angle");
	//this->Arc->SetLabelFormat(this->LabelFormat);
	//  this->Arc->AutoLabelOn();
}

//----------------------------------------------------------------------
keyinAngleRepresentation::~keyinAngleRepresentation()
{
	if (this->Point1Representation)
	{
		this->Point1Representation->Delete();
	}
	if (this->CenterRepresentation)
	{
		this->CenterRepresentation->Delete();
	}
	if (this->Point2Representation)
	{
		this->Point2Representation->Delete();
	}
	this->Ray1->Delete();
	this->Ray2->Delete();
	this->Arc->Delete();
}

void keyinAngleRepresentation::InstantiateHandleRepresentation()
{
	if (!this->Point1Representation)
	{
		this->Point1Representation = keyinHandleRepresentation::New();
	}

	if (!this->CenterRepresentation)
	{
		this->CenterRepresentation = keyinHandleRepresentation::New();
	}

	if (!this->Point2Representation)
	{
		this->Point2Representation = keyinHandleRepresentation::New();
	}
}
int keyinAngleRepresentation::ComputeInteractionState(int vtkNotUsed(X), int vtkNotUsed(Y), int vtkNotUsed(modify))
{
	if (this->Point1Representation == nullptr ||
		this->CenterRepresentation == nullptr ||
		this->Point2Representation == nullptr)
	{
		this->InteractionState = vtkAngleRepresentation::Outside;
		return this->InteractionState;
	}

	int p1State = this->Point1Representation->GetInteractionState();
	int cState = this->CenterRepresentation->GetInteractionState();
	int p2State = this->Point2Representation->GetInteractionState();
	if (p1State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkAngleRepresentation::NearP1;
	}
	else if (cState == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkAngleRepresentation::NearCenter;
	}
	else if (p2State == vtkHandleRepresentation::Nearby)
	{
		this->InteractionState = vtkAngleRepresentation::NearP2;
	}
	else
	{
		this->InteractionState = vtkAngleRepresentation::Outside;
	}

	return this->InteractionState;
}
//----------------------------------------------------------------------
double keyinAngleRepresentation::GetAngle()
{
	return this->Arc->GetAngle();
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetPoint1WorldPosition(double pos[3])
{
	if (this->Point1Representation)
	{
		this->Point1Representation->GetWorldPosition(pos);
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetCenterWorldPosition(double pos[3])
{
	if (this->CenterRepresentation)
	{
		this->CenterRepresentation->GetWorldPosition(pos);
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetPoint2WorldPosition(double pos[3])
{
	if (this->Point2Representation)
	{
		this->Point2Representation->GetWorldPosition(pos);
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::SetPoint1DisplayPosition(double x[3])
{
	if (!this->Point1Representation)
	{
		vtkErrorMacro("SetPoint1DisplayPosition: no point1 representation");
		return;
	}
	this->Point1Representation->SetDisplayPosition(x);
	double p[3];
	this->Point1Representation->GetWorldPosition(p);
	this->Point1Representation->SetWorldPosition(p);
	this->BuildRepresentation();
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::SetCenterDisplayPosition(double x[3])
{
	if (!this->CenterRepresentation)
	{
		vtkErrorMacro("SetCenterDisplayPosition: no center representation");
		return;
	}
	this->CenterRepresentation->SetDisplayPosition(x);
	double p[3];
	this->CenterRepresentation->GetWorldPosition(p);
	this->CenterRepresentation->SetWorldPosition(p);
	this->BuildRepresentation();
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::SetPoint2DisplayPosition(double x[3])
{
	if (!this->Point2Representation)
	{
		vtkErrorMacro("SetPoint2DisplayPosition: no point2 representation");
		return;
	}
	this->Point2Representation->SetDisplayPosition(x);
	double p[3];
	this->Point2Representation->GetWorldPosition(p);
	this->Point2Representation->SetWorldPosition(p);
	this->BuildRepresentation();
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetPoint1DisplayPosition(double pos[3])
{
	if (this->Point1Representation)
	{
		this->Point1Representation->GetDisplayPosition(pos);
		pos[2] = 0.0;
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetCenterDisplayPosition(double pos[3])
{
	if (this->CenterRepresentation)
	{
		this->CenterRepresentation->GetDisplayPosition(pos);
		pos[2] = 0.0;
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::GetPoint2DisplayPosition(double pos[3])
{
	if (this->Point2Representation)
	{
		this->Point2Representation->GetDisplayPosition(pos);
		pos[2] = 0.0;
	}
	else
	{
		pos[0] = pos[1] = pos[2] = 0.0;
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::BuildRepresentation()
{
	if (this->Point1Representation == nullptr ||
		this->CenterRepresentation == nullptr ||
		this->Point2Representation == nullptr ||
		this->Arc == nullptr)
	{
		// for now, return. Could create defaults here.
		return;
	}

	/*if (this->GetMTime() > this->BuildTime || this->Point1Representation->GetMTime() > this->BuildTime || this->CenterRepresentation->GetMTime() > this->BuildTime || 
		this->Point2Representation->GetMTime() > this->BuildTime ||(this->Renderer && this->Renderer->GetVTKWindow() && this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))*/
	{
		this->Superclass::BuildRepresentation();

		// Local coordinate values
		double p1w[3], p2w[3], cw[3], p1d[3], p2d[3], cd[3], vector2[3], vector1[3];
		this->GetPoint1WorldPosition(p1w);
		this->GetCenterWorldPosition(cw);
		this->GetPoint2WorldPosition(p2w);
		//this->GetPoint1DisplayPosition(p1d);
		this->GetCenterDisplayPosition(cd);
		//this->GetPoint2DisplayPosition(p2d);

		vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer, p1w[0], p1w[1], p1w[2], p1d);
		vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer, p2w[0], p2w[1], p2w[2], p2d);

		// Update the rays
		this->Ray1->GetPosition2Coordinate()->SetValue(p1w);
		this->Ray1->GetPositionCoordinate()->SetValue(cw);
		this->Ray2->GetPositionCoordinate()->SetValue(cw);
		this->Ray2->GetPosition2Coordinate()->SetValue(p2w);


		//std::cout << "ARC::  " << p2d[0] << " " << p2d[1] << std::endl;

		// Compute the angle.
		// NOTE: There is some concern that there may be fluctuations in the angle
		// value as the camera moves, etc. This calculation may have to be dampened.
		vector1[0] = p1w[0] - cw[0];
		vector1[1] = p1w[1] - cw[1];
		vector1[2] = p1w[2] - cw[2];
		vector2[0] = p2w[0] - cw[0];
		vector2[1] = p2w[1] - cw[1];
		vector2[2] = p2w[2] - cw[2];
		vtkMath::Normalize(vector1);
		vtkMath::Normalize(vector2);
		double angle = acos(vtkMath::Dot(vector1, vector2));
		char string[512];
		snprintf(string, sizeof(string), this->LabelFormat, vtkMath::DegreesFromRadians(angle));
		this->Arc->SetLabel(string);

		// Place the label and place the arc
		double l1 = sqrt(vtkMath::Distance2BetweenPoints(cd, p1d));
		double l2 = sqrt(vtkMath::Distance2BetweenPoints(cd, p2d));

		// If too small or no render get out
		if (l1 <= 5.0 || l2 <= 5.0 || !this->Renderer)
		{
			this->ArcVisibility = 0;
			return;
		}

		// Place the end points for the arc away from the tip of the two rays
		this->ArcVisibility = 1;
		this->Arc->SetLabelFormat(this->LabelFormat);
		const double rayPosition = 0.20;//------------------------------------»¡Ïß°ë¾¶¶ÌÏß±ÈÀý
		int i;
		double a1[3], a2[3], t1, t2, w1[4], w2[4], radius;
		double ray1[3], ray2[3], v[3], z[3];
		if (l1 < l2)
		{
			radius = rayPosition * l1;
			t1 = rayPosition;
			t2 = (l1 / l2)*rayPosition;
		}
		else
		{
			radius = rayPosition * l2;
			t1 = (l2 / l1)*rayPosition;
			t2 = rayPosition;
		}
		for (i = 0; i < 3; i++)
		{
			ray1[i] = p1d[i] - cd[i];
			ray2[i] = p2d[i] - cd[i];
			a1[i] = cd[i] + t1 * ray1[i];
			a2[i] = cd[i] + t2 * ray2[i];
		}
		double l = sqrt(vtkMath::Distance2BetweenPoints(a1, a2));
		vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, a1[0], a1[1], a1[2], w1);
		vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, a2[0], a2[1], a2[2], w2);
		this->Arc->GetPositionCoordinate()->SetValue(w1);
		this->Arc->GetPosition2Coordinate()->SetValue(w2);
		memcpy(this->Arc->centerPos, cd, 3 * sizeof(double));
		if (l <= 0.0)
		{
			this->Arc->SetRadius(0.0);
		}
		else
		{
			vtkMath::Cross(ray1, ray2, v);
			z[0] = z[1] = 0.0; z[2] = 1.0;
			if (vtkMath::Dot(v, z) > 0.0)
			{
				this->Arc->SetRadius(-radius / l);
			}
			else
			{
				this->Arc->SetRadius(radius / l);
			}
		}
		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
	if (this->Ray1)
	{
		this->Ray1->ReleaseGraphicsResources(w);
	}
	if (this->Ray2)
	{
		this->Ray2->ReleaseGraphicsResources(w);
	}
	if (this->Arc)
	{
		this->Arc->ReleaseGraphicsResources(w);
	}
}

//----------------------------------------------------------------------
int keyinAngleRepresentation::RenderOverlay(vtkViewport *v)
{
	this->BuildRepresentation();

	int count = 0;
	if (this->Ray1 && this->Ray1Visibility)
	{
		count += this->Ray1->RenderOverlay(v);
	}
	if (this->Ray2 && this->Ray2Visibility)
	{
		count += this->Ray2->RenderOverlay(v);
	}
	if (this->Arc && this->ArcVisibility)
	{
		count += this->Arc->RenderOverlay(v);
	}

	return count;
}

//----------------------------------------------------------------------
void keyinAngleRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	os << indent << "Ray1: ";
	if (this->Ray1)
	{
		this->Ray1->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << "(none)\n";
	}

	os << indent << "Ray2: ";
	if (this->Ray2)
	{
		this->Ray2->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << "(none)\n";
	}

	os << indent << "Arc: ";
	if (this->Arc)
	{
		this->Arc->PrintSelf(os, indent.GetNextIndent());
	}
	else
	{
		os << "(none)\n";
	}

}
