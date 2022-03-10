
#include "keyinHandleRepresentation.h"
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkProperty2D.h>
#include <vtkAssemblyPath.h>
#include <vtkMath.h>
#include <vtkInteractorObserver.h>
#include <vtkLine.h>
#include <vtkCoordinate.h>
#include <vtkGlyph2D.h>
#include <vtkCursor2D.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPoints.h>
#include <vtkWindow.h>
#include <vtkCamera.h>
#include <vtkPointPlacer.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>


vtkStandardNewMacro(keyinHandleRepresentation);

vtkCxxSetObjectMacro(keyinHandleRepresentation, Property, vtkProperty2D);
vtkCxxSetObjectMacro(keyinHandleRepresentation, SelectedProperty, vtkProperty2D);
vtkCxxSetObjectMacro(keyinHandleRepresentation, PointPlacer, vtkPointPlacer);


void keyinHandleRepresentation::SetHandleImageFile(const char * handleImageFile)
{
	vtkSmartPointer<vtkPNGReader> pointImage = vtkSmartPointer<vtkPNGReader>::New();
	pointImage->SetFileName(handleImageFile);
	pointImage->Update();
	pointImage->GetOutput()->GetDimensions(imageSize);
	this->pointMapper->SetInputData(pointImage->GetOutput());
}

//----------------------------------------------------------------------
keyinHandleRepresentation::keyinHandleRepresentation()
{
	// Initialize state
	this->InteractionState = vtkHandleRepresentation::Outside;

	// Represent the position of the cursor
	this->FocalPoint = vtkPoints::New();
	this->FocalPoint->SetNumberOfPoints(1);
	this->FocalPoint->SetPoint(0, 0.0, 0.0, 0.0);

	this->FocalData = vtkPolyData::New();
	this->FocalData->SetPoints(this->FocalPoint);

	// The transformation of the cursor will be done via vtkGlyph2D
	// By default a vtkGlyphSOurce2D will be used to define the cursor shape
	vtkCursor2D *cursor2D = vtkCursor2D::New();
	cursor2D->AllOff();
	cursor2D->AxesOn();
	cursor2D->PointOn();
	cursor2D->Update();
	this->CursorShape = cursor2D->GetOutput();
	this->CursorShape->Register(this);
	cursor2D->Delete();

	this->Glypher = vtkGlyph2D::New();
	this->Glypher->SetInputData(this->FocalData);
	this->Glypher->SetSourceData(this->CursorShape);
	this->Glypher->SetVectorModeToVectorRotationOff();
	this->Glypher->ScalingOn();
	this->Glypher->SetScaleModeToDataScalingOff();
	this->Glypher->SetScaleFactor(1.0);

	this->MapperCoordinate = vtkCoordinate::New();
	this->MapperCoordinate->SetCoordinateSystemToDisplay();

	this->pointMapper = vtkImageMapper::New();
	this->pointMapper->SetColorWindow(256);
	this->pointMapper->SetColorLevel(128);

	this->Mapper = vtkPolyDataMapper2D::New();
	this->Mapper->SetInputConnection(this->Glypher->GetOutputPort());
	this->Mapper->SetTransformCoordinate(this->MapperCoordinate);

	// Set up the initial properties
	this->CreateDefaultProperties();

	this->Actor = vtkActor2D::New();
	this->Actor->SetMapper(this->pointMapper);
	this->Actor->SetProperty(this->Property);

	// The size of the hot spot
	this->WaitingForMotion = 0;
	this->ConstraintAxis = -1;
}

//----------------------------------------------------------------------
keyinHandleRepresentation::~keyinHandleRepresentation()
{
	this->FocalPoint->Delete();
	this->FocalData->Delete();

	this->CursorShape->Delete();
	this->Glypher->Delete();
	this->MapperCoordinate->Delete();
	this->Mapper->Delete();
	this->Actor->Delete();

	this->Property->Delete();
	this->SelectedProperty->Delete();
	this->pointMapper->Delete();
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::SetCursorShape(vtkPolyData *shape)
{
	if (shape != this->CursorShape)
	{
		if (this->CursorShape)
		{
			this->CursorShape->Delete();
		}
		this->CursorShape = shape;
		if (this->CursorShape)
		{
			this->CursorShape->Register(this);
		}
		this->Glypher->SetSourceData(this->CursorShape);
		this->Modified();
	}
}

//----------------------------------------------------------------------
vtkPolyData *keyinHandleRepresentation::GetCursorShape()
{
	return this->CursorShape;
}

//-------------------------------------------------------------------------
double* keyinHandleRepresentation::GetBounds()
{
	return nullptr;
}

//-------------------------------------------------------------------------
void keyinHandleRepresentation::SetDisplayPosition(double p[3])
{
	this->Superclass::SetDisplayPosition(p);
	this->FocalPoint->SetPoint(0, p);
	this->FocalPoint->Modified();

	if (this->PointPlacer)
	{
		// The point placer will compute the world position for us.
		return;
	}

	double w[4];
	if (this->Renderer)
	{
		vtkInteractorObserver::ComputeDisplayToWorld(
			this->Renderer, p[0], p[1], p[2], w);
		this->SetWorldPosition(w);
	}
}

//-------------------------------------------------------------------------
int keyinHandleRepresentation::ComputeInteractionState(int X, int Y, int vtkNotUsed(modify))
{
	double pos[3], xyz[3];
	this->FocalPoint->GetPoint(0, pos);
	xyz[0] = static_cast<double>(X);
	xyz[1] = static_cast<double>(Y);
	xyz[2] = pos[2];
	
	this->VisibilityOn();
	double tol2 = this->Tolerance * this->Tolerance;
	if (vtkMath::Distance2BetweenPoints(xyz, pos) <= tol2)
	{
		this->InteractionState = vtkHandleRepresentation::Nearby;
		this->Renderer->AddActor2D(this->Actor);
	}
	else
	{
		this->InteractionState = vtkHandleRepresentation::Outside;
		this->Renderer->RemoveActor2D(this->Actor);
		if (this->ActiveRepresentation)
		{
			this->VisibilityOff();
		}
	}

	return this->InteractionState;
}

//-------------------------------------------------------------------------
int keyinHandleRepresentation::DetermineConstraintAxis(int constraint,double eventPos[2])
{
	// Look for trivial cases: either not constrained or already constrained
	if (!this->Constrained)
	{
		return -1;
	}
	else if (constraint >= 0 && constraint < 3)
	{
		return constraint;
	}

	// Okay, figure out constraint based on mouse motion
	double dpos[2];
	dpos[0] = fabs(eventPos[0] - this->StartEventPosition[0]);
	dpos[1] = fabs(eventPos[1] - this->StartEventPosition[1]);

	return (dpos[0] > dpos[1] ? 0 : 1);
}

//----------------------------------------------------------------------
// Record the current event position, and the rectilinear wipe position.
void keyinHandleRepresentation::StartWidgetInteraction(double startEventPos[2])
{
	this->StartEventPosition[0] = startEventPos[0];
	this->StartEventPosition[1] = startEventPos[1];
	this->StartEventPosition[2] = 0.0;

	this->LastEventPosition[0] = startEventPos[0];
	this->LastEventPosition[1] = startEventPos[1];

	this->ConstraintAxis = -1;
	this->WaitCount = 0;
	if (this->Constrained)
	{
		this->WaitingForMotion = 1;
	}
	else
	{
		this->WaitingForMotion = 0;
	}
}


//----------------------------------------------------------------------
// Based on the displacement vector (computed in display coordinates) and
// the cursor state (which corresponds to which part of the widget has been
// selected), the widget points are modified.
// First construct a local coordinate system based on the display coordinates
// of the widget.
void keyinHandleRepresentation::WidgetInteraction(double eventPos[2])
{
	// Process the motion
	if (this->InteractionState == vtkHandleRepresentation::Selecting ||
		this->InteractionState == vtkHandleRepresentation::Translating)
	{
		if (!this->WaitingForMotion || this->WaitCount++ > 1)
		{

			this->ConstraintAxis =
				this->DetermineConstraintAxis(this->ConstraintAxis, eventPos);
			this->Translate(eventPos);
		}
	}

	else if (this->InteractionState == vtkHandleRepresentation::Scaling)
	{
		this->Scale(eventPos);
	}

	// Book keeping
	this->LastEventPosition[0] = eventPos[0];
	this->LastEventPosition[1] = eventPos[1];

	this->Modified();
}

//----------------------------------------------------------------------
// Translate everything
void keyinHandleRepresentation::Translate(double eventPos[2])
{
	double pos[3], dpos[2];
	this->FocalPoint->GetPoint(0, pos);
	dpos[0] = eventPos[0] - pos[0];
	dpos[1] = eventPos[1] - pos[1];

	if (this->ConstraintAxis >= 0)
	{
		pos[this->ConstraintAxis] += dpos[this->ConstraintAxis];
	}
	else
	{
		pos[0] += dpos[0];
		pos[1] += dpos[1];
	}
	this->SetDisplayPosition(pos);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::Scale(double eventPos[2])
{
	// Get the current scale factor
	double sf = this->Glypher->GetScaleFactor();

	// Compute the scale factor
	int *size = this->Renderer->GetSize();
	double dPos = static_cast<double>(eventPos[1] - this->LastEventPosition[1]);
	sf *= (1.0 + 2.0*(dPos / size[1])); //scale factor of 2.0 is arbitrary

	// Scale the handle
	this->Glypher->SetScaleFactor(sf);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::Highlight(int highlight)
{
	if (highlight)
	{
		this->Actor->SetProperty(this->SelectedProperty);
	}
	else
	{
		this->Actor->SetProperty(this->Property);
	}
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::CreateDefaultProperties()
{
	this->Property = vtkProperty2D::New();
	this->Property->SetColor(1.0, 1.0, 1.0);
	this->Property->SetLineWidth(1.0);

	this->SelectedProperty = vtkProperty2D::New();
	this->SelectedProperty->SetColor(0.0, 1.0, 0.0);
	this->SelectedProperty->SetLineWidth(2.0);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::BuildRepresentation()
{
	/*if (this->GetMTime() > this->BuildTime ||
		(this->Renderer && this->Renderer->GetActiveCamera() &&
			this->Renderer->GetActiveCamera()->GetMTime() > this->BuildTime) ||
			(this->Renderer && this->Renderer->GetVTKWindow() &&
				this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime))*/
	{
		double p[3];
		this->GetDisplayPosition(p);
		this->FocalPoint->SetPoint(0, p);
		this->FocalPoint->Modified();

		this->Actor->SetPosition(p[0]-imageSize[0]/2,p[1]-imageSize[1]/2);

		this->BuildTime.Modified();
	}
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::ShallowCopy(vtkProp *prop)
{
	keyinHandleRepresentation *rep =
		keyinHandleRepresentation::SafeDownCast(prop);
	if (rep)
	{
		this->SetCursorShape(rep->GetCursorShape());
		this->SetProperty(rep->GetProperty());
		this->SetSelectedProperty(rep->GetSelectedProperty());
		this->Actor->SetProperty(this->Property);
	}
	this->Superclass::ShallowCopy(prop);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::DeepCopy(vtkProp *prop)
{
	keyinHandleRepresentation *rep =
		keyinHandleRepresentation::SafeDownCast(prop);
	if (rep)
	{
		this->SetCursorShape(rep->GetCursorShape());
		this->Property->DeepCopy(rep->GetProperty());
		this->SelectedProperty->DeepCopy(rep->GetSelectedProperty());
		this->Actor->SetProperty(this->Property);
	}
	this->Superclass::DeepCopy(prop);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::GetActors2D(vtkPropCollection *pc)
{
	this->Actor->GetActors2D(pc);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::ReleaseGraphicsResources(vtkWindow *win)
{
	this->Renderer->RemoveActor2D(this->Actor);
	this->Actor->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int keyinHandleRepresentation::RenderOverlay(vtkViewport *viewport)
{
	this->BuildRepresentation();
	//this->Renderer->AddActor2D(this->Actor);
	return 0;// this->Actor->RenderOverlay(viewport);
}

//----------------------------------------------------------------------
void keyinHandleRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	if (this->Property)
	{
		os << indent << "Property: " << this->Property << "\n";
	}
	else
	{
		os << indent << "Property: (none)\n";
	}

	if (this->SelectedProperty)
	{
		os << indent << "Selected Property: " << this->SelectedProperty << "\n";
	}
	else
	{
		os << indent << "Selected Property: (none)\n";
	}

	if (this->CursorShape)
	{
		os << indent << "Cursor Shape: " << this->CursorShape << "\n";
	}
	else
	{
		os << indent << "Cursor Shape: (none)\n";
	}

}
