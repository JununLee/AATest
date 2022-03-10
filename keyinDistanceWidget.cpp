
#include "keyinDistanceWidget.h"
#include <vtkDistanceRepresentation2D.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkHandleWidget.h>
#include <vtkCoordinate.h>
#include <vtkHandleRepresentation.h>
#include <vtkNew.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>
#include <vtkWidgetEventTranslator.h>
#include "keyinDistanceRepresentation.h"
#include <vtkEvent.h>
#include "keyinDistanceWidget.h"
#include <vtkRenderWindow.h>

vtkStandardNewMacro(keyinDistanceWidget);


// The distance widget observes its two handles.
// Here we create the command/observer classes to respond to the
// handle widgets.
class vtkDistanceWidgetCallback : public vtkCommand
{
public:
	static vtkDistanceWidgetCallback *New()
	{
		return new vtkDistanceWidgetCallback;
	}
	void Execute(vtkObject*, unsigned long eventId, void*) override
	{
		switch (eventId)
		{
		case vtkCommand::StartInteractionEvent:
			this->DistanceWidget->StartDistanceInteraction(this->HandleNumber);
			break;
		case vtkCommand::InteractionEvent:
			this->DistanceWidget->DistanceInteraction(this->HandleNumber);
			break;
		case vtkCommand::EndInteractionEvent:
			this->DistanceWidget->EndDistanceInteraction(this->HandleNumber);
			break;
		}
	}
	int HandleNumber;
	keyinDistanceWidget *DistanceWidget;
};


//----------------------------------------------------------------------
keyinDistanceWidget::keyinDistanceWidget()
{
	this->ManagesCursor = 0;

	this->WidgetState = keyinDistanceWidget::Start;
	this->CurrentHandle = 0;

	// The widgets for moving the end points. They observe this widget (i.e.,
	// this widget is the parent to the handles).
	this->Point1Widget = vtkHandleWidget::New();
	this->Point1Widget->SetParent(this);
	this->Point2Widget = vtkHandleWidget::New();
	this->Point2Widget->SetParent(this);

	// Set up the callbacks on the two handles
	this->DistanceWidgetCallback1 = vtkDistanceWidgetCallback::New();
	this->DistanceWidgetCallback1->HandleNumber = 0;
	this->DistanceWidgetCallback1->DistanceWidget = this;
	this->Point1Widget->AddObserver(vtkCommand::StartInteractionEvent, this->DistanceWidgetCallback1,
		this->Priority);
	this->Point1Widget->AddObserver(vtkCommand::InteractionEvent, this->DistanceWidgetCallback1,
		this->Priority);
	this->Point1Widget->AddObserver(vtkCommand::EndInteractionEvent, this->DistanceWidgetCallback1,
		this->Priority);

	this->DistanceWidgetCallback2 = vtkDistanceWidgetCallback::New();
	this->DistanceWidgetCallback2->HandleNumber = 1;
	this->DistanceWidgetCallback2->DistanceWidget = this;
	this->Point2Widget->AddObserver(vtkCommand::StartInteractionEvent, this->DistanceWidgetCallback2,
		this->Priority);
	this->Point2Widget->AddObserver(vtkCommand::InteractionEvent, this->DistanceWidgetCallback2,
		this->Priority);
	this->Point2Widget->AddObserver(vtkCommand::EndInteractionEvent, this->DistanceWidgetCallback2,
		this->Priority);


	// These are the event callbacks supported by this widget
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkWidgetEvent::AddPoint,
		this, keyinDistanceWidget::AddPointAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, keyinDistanceWidget::MoveAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, keyinDistanceWidget::EndSelectAction);

	this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
		vtkEvent::NoModifier, 8, 1, "BackSpace",
		vtkWidgetEvent::Delete,
		this, keyinDistanceWidget::DeleteWidget);

}

//----------------------------------------------------------------------
keyinDistanceWidget::~keyinDistanceWidget()
{
	this->Point1Widget->RemoveObserver(this->DistanceWidgetCallback1);
	this->Point1Widget->Delete();
	this->DistanceWidgetCallback1->Delete();

	this->Point2Widget->RemoveObserver(this->DistanceWidgetCallback2);
	this->Point2Widget->Delete();
	this->DistanceWidgetCallback2->Delete();
}

//----------------------------------------------------------------------
void keyinDistanceWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		this->WidgetRep = vtkDistanceRepresentation2D::New();
	}
	reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep)->
		InstantiateHandleRepresentation();
}

//----------------------------------------------------------------------
void keyinDistanceWidget::SetEnabled(int enabling)
{
	// The handle widgets are not actually enabled until they are placed.
	// The handle widgets take their representation from the
	// vtkDistanceRepresentation.
	if (enabling)
	{
		if (this->WidgetState == keyinDistanceWidget::Start)
		{
			reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep)->
				VisibilityOff();
		}
		else
		{
			// The interactor must be set prior to enabling the widget.
			if (this->Interactor)
			{
				this->Point1Widget->SetInteractor(this->Interactor);
				this->Point2Widget->SetInteractor(this->Interactor);
			}

			this->Point1Widget->SetEnabled(1);
			this->Point2Widget->SetEnabled(1);
		}
	}

	if (enabling) //----------------
	{
		if (this->Enabled) //already enabled, just return
		{
			return;
		}

		if (!this->Interactor)
		{
			vtkErrorMacro(<< "The interactor must be set prior to enabling the widget");
			return;
		}

		int X = this->Interactor->GetEventPosition()[0];
		int Y = this->Interactor->GetEventPosition()[1];

		if (!this->CurrentRenderer)
		{
			this->SetCurrentRenderer(this->Interactor->FindPokedRenderer(X, Y));
			if (this->CurrentRenderer == nullptr)
			{
				return;
			}
		}

		// We're ready to enable
		this->Enabled = 1;
		this->CreateDefaultRepresentation();
		this->WidgetRep->SetRenderer(this->CurrentRenderer);

		// Set the renderer, interactor and representation on the two handle
		// widgets.
		this->Point1Widget->SetRepresentation(
			reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep)->
			GetPoint1Representation());
		this->Point1Widget->SetInteractor(this->Interactor);
		this->Point1Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

		this->Point2Widget->SetRepresentation(
			reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep)->
			GetPoint2Representation());
		this->Point2Widget->SetInteractor(this->Interactor);
		this->Point2Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

		// listen for the events found in the EventTranslator
		if (!this->Parent)
		{
			this->EventTranslator->AddEventsToInteractor(this->Interactor,
				this->EventCallbackCommand, this->Priority);
		}
		else
		{
			this->EventTranslator->AddEventsToParent(this->Parent,
				this->EventCallbackCommand, this->Priority);
		}

		if (this->ManagesCursor)
		{
			this->WidgetRep->ComputeInteractionState(X, Y);
			this->SetCursor(this->WidgetRep->GetInteractionState());
		}

		this->WidgetRep->BuildRepresentation();
		this->CurrentRenderer->AddViewProp(this->WidgetRep);

		if (this->WidgetState == keyinDistanceWidget::Start)
		{
			reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep)->
				VisibilityOff();
		}
		else
		{
			this->Point1Widget->SetEnabled(1);
			this->Point2Widget->SetEnabled(1);
		}

		this->InvokeEvent(vtkCommand::EnableEvent, nullptr);
	}

	else //disabling------------------
	{
		vtkDebugMacro(<< "Disabling widget");

		if (!this->Enabled) //already disabled, just return
		{
			return;
		}

		this->Enabled = 0;

		// don't listen for events any more
		if (!this->Parent)
		{
			this->Interactor->RemoveObserver(this->EventCallbackCommand);
		}
		else
		{
			this->Parent->RemoveObserver(this->EventCallbackCommand);
		}

		this->CurrentRenderer->RemoveViewProp(this->WidgetRep);

		this->Point1Widget->SetEnabled(0);
		this->Point2Widget->SetEnabled(0);

		this->InvokeEvent(vtkCommand::DisableEvent, nullptr);
		this->SetCurrentRenderer(nullptr);
	}

	// Should only render if there is no parent
	if (this->Interactor && !this->Parent)
	{
		this->Interactor->Render();
	}
}

// The following methods are the callbacks that the measure widget responds to
//-------------------------------------------------------------------------
void keyinDistanceWidget::AddPointAction(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);
	keyinDistanceRepresentation *rep = reinterpret_cast<keyinDistanceRepresentation*>(self->WidgetRep);
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;
	if (rep->GetAxis()->MouseOverLabel(pos)) {
		self->moveLabel = true;
		self->lastPos[0] = pos[0];
		self->lastPos[1] = pos[1];
	}
	// Freshly enabled and placing the first point
	else if (self->WidgetState == keyinDistanceWidget::Start)
	{
		self->GrabFocus(self->EventCallbackCommand);
		self->WidgetState = keyinDistanceWidget::Define;
		self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
		reinterpret_cast<vtkDistanceRepresentation*>(self->WidgetRep)->VisibilityOn();
		double e[2];
		e[0] = static_cast<double>(X);
		e[1] = static_cast<double>(Y);
		reinterpret_cast<vtkDistanceRepresentation*>(self->WidgetRep)->StartWidgetInteraction(e);
		self->CurrentHandle = 0;
		self->InvokeEvent(vtkCommand::PlacePointEvent, &(self->CurrentHandle));
	}

	// Placing the second point is easy
	else if (self->WidgetState == keyinDistanceWidget::Define)
	{
		self->CurrentHandle = 1;
		self->InvokeEvent(vtkCommand::PlacePointEvent, &(self->CurrentHandle));
		self->WidgetState = keyinDistanceWidget::Manipulate;
		self->Point1Widget->SetEnabled(1);
		self->Point2Widget->SetEnabled(1);
		self->CurrentHandle = -1;
		self->ReleaseFocus();
		self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	}

	// Maybe we are trying to manipulate the widget handles
	else //if ( self->WidgetState == keyinDistanceWidget::Manipulate )
	{
		int state = self->WidgetRep->ComputeInteractionState(X, Y);

		if (state == vtkDistanceRepresentation::Outside)
		{
			self->CurrentHandle = -1;
			return;
		}

		self->GrabFocus(self->EventCallbackCommand);
		if (state == vtkDistanceRepresentation::NearP1)
		{
			self->CurrentHandle = 0;
		}
		else if (state == vtkDistanceRepresentation::NearP2)
		{
			self->CurrentHandle = 1;
		}
		self->InvokeEvent(vtkCommand::LeftButtonPressEvent, nullptr);
	}

	// Clean up
	self->EventCallbackCommand->SetAbortFlag(1);
	self->Render();
}

//-------------------------------------------------------------------------
void keyinDistanceWidget::AddPointAction3D(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);

	// Freshly enabled and placing the first point
	if (self->WidgetState == keyinDistanceWidget::Start)
	{
		self->WidgetState = keyinDistanceWidget::Define;
		self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
		reinterpret_cast<vtkDistanceRepresentation*>(self->WidgetRep)->VisibilityOn();
		self->WidgetRep->StartComplexInteraction(
			self->Interactor, self, vtkWidgetEvent::AddPoint, self->CallData);
		self->CurrentHandle = 0;
		self->InvokeEvent(vtkCommand::PlacePointEvent, &(self->CurrentHandle));
		self->EventCallbackCommand->SetAbortFlag(1);
	}

	// Placing the second point is easy
	else if (self->WidgetState == keyinDistanceWidget::Define)
	{
		self->CurrentHandle = 1;
		self->InvokeEvent(vtkCommand::PlacePointEvent, &(self->CurrentHandle));
		self->WidgetState = keyinDistanceWidget::Manipulate;
		self->Point1Widget->SetEnabled(1);
		self->Point2Widget->SetEnabled(1);
		self->CurrentHandle = -1;
		self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
		self->EventCallbackCommand->SetAbortFlag(1);
	}

	// Maybe we are trying to manipulate the widget handles
	else //if ( self->WidgetState == keyinDistanceWidget::Manipulate )
	{
		int state = self->WidgetRep->ComputeComplexInteractionState(
			self->Interactor, self, vtkWidgetEvent::AddPoint, self->CallData);

		if (state == vtkDistanceRepresentation::Outside)
		{
			self->CurrentHandle = -1;
			return;
		}

		if (state == vtkDistanceRepresentation::NearP1)
		{
			self->CurrentHandle = 0;
		}
		else if (state == vtkDistanceRepresentation::NearP2)
		{
			self->CurrentHandle = 1;
		}
		self->InvokeEvent(vtkCommand::Button3DEvent, self->CallData);
		self->EventCallbackCommand->SetAbortFlag(1);
	}

	// Clean up
	self->Render();
}

//-------------------------------------------------------------------------
void keyinDistanceWidget::MoveAction(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);

	// Do nothing if in start mode or valid handle not selected
	if (self->WidgetState == keyinDistanceWidget::Start)
	{
		return;
	}

	// Delegate the event consistent with the state
	if (self->WidgetState == keyinDistanceWidget::Define)
	{
		int X = self->Interactor->GetEventPosition()[0];
		int Y = self->Interactor->GetEventPosition()[1];
		double e[2];
		e[0] = static_cast<double>(X);
		e[1] = static_cast<double>(Y);
		reinterpret_cast<vtkDistanceRepresentation*>(self->WidgetRep)->WidgetInteraction(e);
		self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		self->EventCallbackCommand->SetAbortFlag(1);
	}
	else //must be moving a handle, invoke a event for the handle widgets
	{
		self->InvokeEvent(vtkCommand::MouseMoveEvent, nullptr);
	}

	self->EventCallbackCommand->SetAbortFlag(1);
	keyinDistanceRepresentation *rep = reinterpret_cast<keyinDistanceRepresentation*>(self->WidgetRep);
	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;


	if (self->moveLabel) {

		double offset[2];
		offset[0] = pos[0] - self->lastPos[0];
		offset[1] = pos[1] - self->lastPos[1];
		rep->GetAxis()->LabelOffset(offset);
		self->lastPos[0] = pos[0];
		self->lastPos[1] = pos[1];
	}
	else
	{
		if (rep->GetAxis()->MouseOverLabel(pos)) {
			self->RequestCursorShape(VTK_CURSOR_HAND);
		}
		else
		{
			self->RequestCursorShape(VTK_CURSOR_DEFAULT);
		}
	}


	self->WidgetRep->BuildRepresentation();
	self->Render();



}

//-------------------------------------------------------------------------
void keyinDistanceWidget::MoveAction3D(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);

	// Do nothing if in start mode or valid handle not selected
	if (self->WidgetState == keyinDistanceWidget::Start)
	{
		return;
	}

	// Delegate the event consistent with the state
	if (self->WidgetState == keyinDistanceWidget::Define)
	{
		self->WidgetRep->ComplexInteraction(
			self->Interactor, self, vtkWidgetEvent::Move3D, self->CallData);
		self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	}
	else //must be moving a handle, invoke a event for the handle widgets
	{
		self->InvokeEvent(vtkCommand::Move3DEvent, self->CallData);
	}

	self->WidgetRep->BuildRepresentation();
	self->Render();
}

//-------------------------------------------------------------------------
void keyinDistanceWidget::EndSelectAction(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);

	self->moveLabel = false;

	// Do nothing if outside
	if (self->WidgetState == keyinDistanceWidget::Start ||
		self->WidgetState == keyinDistanceWidget::Define ||
		self->CurrentHandle < 0)
	{
		return;
	}

	self->ReleaseFocus();
	self->InvokeEvent(vtkCommand::LeftButtonReleaseEvent, nullptr);
	self->CurrentHandle = -1;
	self->WidgetRep->BuildRepresentation();
	self->EventCallbackCommand->SetAbortFlag(1);
	self->Render();
}

//-------------------------------------------------------------------------
void keyinDistanceWidget::EndSelectAction3D(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);

	// Do nothing if outside
	if (self->WidgetState == keyinDistanceWidget::Start ||
		self->WidgetState == keyinDistanceWidget::Define ||
		self->CurrentHandle < 0)
	{
		return;
	}

	self->ReleaseFocus();
	self->InvokeEvent(vtkCommand::Button3DEvent, self->CallData);
	self->CurrentHandle = -1;
	self->WidgetRep->BuildRepresentation();
	self->EventCallbackCommand->SetAbortFlag(1);
	self->Render();
}

void keyinDistanceWidget::DeleteWidget(vtkAbstractWidget *w)
{
	keyinDistanceWidget *self = reinterpret_cast<keyinDistanceWidget*>(w);
	keyinDistanceRepresentation *rep = reinterpret_cast<keyinDistanceRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;
	if (rep->GetAxis()->MouseOverLabel(pos)) {
		self->SetEnabled(0);
		self->Point1Widget->Delete();
		self->Point2Widget->Delete();
		self->RequestCursorShape(VTK_CURSOR_DEFAULT);
	}
}

// These are callbacks that are active when the user is manipulating the
// handles of the measure widget.
//----------------------------------------------------------------------
void keyinDistanceWidget::StartDistanceInteraction(int)
{
	this->Superclass::StartInteraction();
	this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
}

//----------------------------------------------------------------------
void keyinDistanceWidget::DistanceInteraction(int)
{
	this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
}

//----------------------------------------------------------------------
void keyinDistanceWidget::EndDistanceInteraction(int)
{
	this->Superclass::EndInteraction();
	this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
}

bool keyinDistanceWidget::MouseOverLabel(keyinDistanceWidget * self)
{
	keyinDistanceRepresentation *rep = reinterpret_cast<keyinDistanceRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;
	return rep->GetAxis()->MouseOverLabel(pos);
}

//----------------------------------------------------------------------
void keyinDistanceWidget::SetProcessEvents(vtkTypeBool pe)
{
	this->Superclass::SetProcessEvents(pe);

	this->Point1Widget->SetProcessEvents(pe);
	this->Point2Widget->SetProcessEvents(pe);
}

//----------------------------------------------------------------------
void keyinDistanceWidget::SetWidgetStateToStart()
{
	this->WidgetState = keyinDistanceWidget::Start;
	this->CurrentHandle = -1;
	this->ReleaseFocus();
	this->GetRepresentation()->BuildRepresentation(); // update this->Distance
	this->SetEnabled(this->GetEnabled()); // show/hide the handles properly
}

//----------------------------------------------------------------------
void keyinDistanceWidget::SetWidgetStateToManipulate()
{
	this->WidgetState = keyinDistanceWidget::Manipulate;
	this->CurrentHandle = -1;
	this->ReleaseFocus();
	this->GetRepresentation()->BuildRepresentation(); // update this->Distance
	this->SetEnabled(this->GetEnabled()); // show/hide the handles properly
}

//----------------------------------------------------------------------
void keyinDistanceWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);
}
