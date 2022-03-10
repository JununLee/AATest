
#include "keyinContourWidget.h"

#include "keyinGlyphContourRepresentation.h"
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkEvent.h>
#include <vtkWidgetEvent.h>
#include <vtkPolyData.h>

vtkStandardNewMacro(keyinContourWidget);

//----------------------------------------------------------------------
keyinContourWidget::keyinContourWidget()
{
	this->ManagesCursor = 0;
	this->WidgetState = keyinContourWidget::Start;
	this->CurrentHandle = 0;
	this->AllowNodePicking = 0;
	this->FollowCursor = 0;
	this->ContinuousDraw = 0;
	this->ContinuousActive = 0;

	// These are the event callbacks supported by this widget
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
		vtkWidgetEvent::Select,
		this, keyinContourWidget::SelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
		vtkWidgetEvent::AddFinalPoint,
		this, keyinContourWidget::AddFinalPointAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
		vtkWidgetEvent::Move,
		this, keyinContourWidget::MoveAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
		vtkWidgetEvent::EndSelect,
		this, keyinContourWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
		vtkEvent::NoModifier, 127, 1, "Delete",
		vtkWidgetEvent::Delete,
		this, keyinContourWidget::DeleteAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
		vtkEvent::NoModifier, 8, 1, "BackSpace",
		vtkWidgetEvent::Delete,
		this, keyinContourWidget::DeleteAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
		vtkEvent::ShiftModifier, 127, 1, "Delete",
		vtkWidgetEvent::Reset,
		this, keyinContourWidget::ResetAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
		vtkWidgetEvent::Translate,
		this, keyinContourWidget::TranslateContourAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent,
		vtkWidgetEvent::EndTranslate,
		this, keyinContourWidget::EndSelectAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
		vtkWidgetEvent::Scale,
		this, keyinContourWidget::ScaleContourAction);
	this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
		vtkWidgetEvent::EndScale,
		this, keyinContourWidget::EndSelectAction);

	this->CreateDefaultRepresentation();
}

//----------------------------------------------------------------------
keyinContourWidget::~keyinContourWidget() = default;

//----------------------------------------------------------------------
void keyinContourWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		keyinGlyphContourRepresentation *rep =
			keyinGlyphContourRepresentation::New();

		this->WidgetRep = rep;

		vtkSphereSource *ss = vtkSphereSource::New();
		ss->SetRadius(0.5);
		ss->Update();
		rep->SetActiveCursorShape(ss->GetOutput());
		ss->Delete();

		rep->GetProperty()->SetColor(0.25, 1.0, 0.25);

		vtkProperty *property =
			vtkProperty::SafeDownCast(rep->GetActiveProperty());
		if (property)
		{
			property->SetRepresentationToSurface();
			property->SetAmbient(0.1);
			property->SetDiffuse(0.9);
			property->SetSpecular(0.0);
		}
	}
}

//----------------------------------------------------------------------
void keyinContourWidget::CloseLoop()
{
	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep);
	if (!rep->GetClosedLoop() && rep->GetNumberOfNodes() > 1)
	{
		this->WidgetState = keyinContourWidget::Manipulate;
		rep->ClosedLoopOn();
		this->Render();
	}
}

//----------------------------------------------------------------------
void keyinContourWidget::SetEnabled(int enabling)
{
	// The handle widgets are not actually enabled until they are placed.
	// The handle widgets take their representation from the keyinContourRepresentation.
	if (enabling)
	{
		if (this->WidgetState == keyinContourWidget::Start)
		{
			reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep)->VisibilityOff();
		}
		else
		{
			reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep)->VisibilityOn();
		}
	}

	this->Superclass::SetEnabled(enabling);
}

// The following methods are the callbacks that the contour widget responds to.
//-------------------------------------------------------------------------
void keyinContourWidget::SelectAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);
	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;

	if (self->ContinuousDraw)
	{
		self->ContinuousActive = 0;
	}

	switch (self->WidgetState)
	{
	case keyinContourWidget::Start:
	case keyinContourWidget::Define:
	{
		// If we are following the cursor, let's add 2 nodes rightaway, on the
		// first click. The second node is the one that follows the cursor
		// around.
		if ((self->FollowCursor || self->ContinuousDraw) && (rep->GetNumberOfNodes() == 0))
		{
			self->AddNode();
		}
		self->AddNode();
		if (self->ContinuousDraw)
		{
			self->ContinuousActive = 1;
		}
		break;
	}

	case keyinContourWidget::Manipulate:
	{
		return;
		if (rep->ActivateNode(X, Y))
		{
			self->Superclass::StartInteraction();
			self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
			self->StartInteraction();
			rep->SetCurrentOperationToTranslate();
			rep->StartWidgetInteraction(pos);
			self->EventCallbackCommand->SetAbortFlag(1);
		}
		else if (rep->AddNodeOnContour(X, Y))//Ôö¼Óµã
		{
			if (rep->ActivateNode(X, Y))
			{
				rep->SetCurrentOperationToTranslate();
				rep->StartWidgetInteraction(pos);
			}
			self->EventCallbackCommand->SetAbortFlag(1);
		}
		else if (!rep->GetNeedToRender())
		{
			rep->SetRebuildLocator(true);
		}
		break;
	}
	}

	if (rep->GetNeedToRender())
	{
		self->Render();
		rep->NeedToRenderOff();
	}
}

//-------------------------------------------------------------------------
void keyinContourWidget::AddFinalPointAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);
	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	if (self->WidgetState != keyinContourWidget::Manipulate &&
		rep->GetNumberOfNodes() >= 1)
	{
		// In follow cursor and continuous draw mode, the "extra" node
		// has already been added for us.
		if (!self->FollowCursor && !self->ContinuousDraw)
		{
			self->AddNode();
		}
		
		if (self->ContinuousDraw)
		{
			self->ContinuousActive = 0;
		}
		self->WidgetState = keyinContourWidget::Manipulate;


		self->EventCallbackCommand->SetAbortFlag(1);
		self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
	}

	if (rep->GetNeedToRender())
	{
		self->Render();
		rep->NeedToRenderOff();
	}
}
//------------------------------------------------------------------------
void keyinContourWidget::AddNode()
{
	int X = this->Interactor->GetEventPosition()[0];
	int Y = this->Interactor->GetEventPosition()[1];

	// If the rep already has at least 2 nodes, check how close we are to
	// the first
	keyinContourRepresentation* rep =
		reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep);

	

	int numNodes = rep->GetNumberOfNodes();
	if (numNodes == 0) {
		if (startCallback != nullptr) {

			startCallback();
		}
	}
	if (numNodes > 1)
	{
		int pixelTolerance = rep->GetPixelTolerance();
		int pixelTolerance2 = pixelTolerance * pixelTolerance;

		double displayPos[2];
		if (!rep->GetNthNodeDisplayPosition(0, displayPos))
		{
			vtkErrorMacro("Can't get first node display position!");
			return;
		}

		// if in continuous draw mode, we don't want to close the loop until we are at least
		// numNodes > pixelTolerance away

		int distance2 = static_cast<int>((X - displayPos[0]) * (X - displayPos[0]) +
			(Y - displayPos[1]) * (Y - displayPos[1]));

		if ((distance2 < pixelTolerance2 && numNodes > 2) ||
			(this->ContinuousDraw && numNodes > pixelTolerance && distance2 < pixelTolerance2))
		{
			// yes - we have made a loop. Stop defining and switch to
			// manipulate mode
			this->WidgetState = keyinContourWidget::Manipulate;

			if (completeCallback != nullptr) {

				completeCallback(rep);
			}
			rep->ClosedLoopOn();
			this->Render();
			this->EventCallbackCommand->SetAbortFlag(1);
			this->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);
			return;
		}
	}

	if (rep->AddNodeAtDisplayPosition(X, Y))
	{
		if (this->WidgetState == keyinContourWidget::Start)
		{
			this->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
		}

		this->WidgetState = keyinContourWidget::Define;
		rep->VisibilityOn();
		this->EventCallbackCommand->SetAbortFlag(1);
		this->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	}
}

//-------------------------------------------------------------------------
// Note that if you select the contour at a location that is not moused over
// a control point, the translate action makes the closest contour node
// jump to the current mouse location. Perhaps we should either
// (a) Disable translations when not moused over a control point
// (b) Fix the jumping behaviour by calculating motion vectors from the start
//     of the interaction.
void keyinContourWidget::TranslateContourAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);
	return;
	if (self->WidgetState != keyinContourWidget::Manipulate)
	{
		return;
	}

	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;

	if (rep->ActivateNode(X, Y))
	{
		self->Superclass::StartInteraction();
		self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
		self->StartInteraction();
		rep->SetCurrentOperationToShift(); // Here
		rep->StartWidgetInteraction(pos);
		self->EventCallbackCommand->SetAbortFlag(1);
	}
	else
	{
		double p[3];
		int idx;
		if (rep->FindClosestPointOnContour(X, Y, p, &idx))
		{
			rep->GetNthNodeDisplayPosition(idx, pos);
			rep->ActivateNode(pos);
			self->Superclass::StartInteraction();
			self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
			self->StartInteraction();
			rep->SetCurrentOperationToShift(); // Here
			rep->StartWidgetInteraction(pos);
			self->EventCallbackCommand->SetAbortFlag(1);
		}
	}

	if (rep->GetNeedToRender())
	{
		self->Render();
		rep->NeedToRenderOff();
	}
}
//-------------------------------------------------------------------------
// Note that if you select the contour at a location that is not moused over
// a control point, the scale action makes the closest contour node
// jump to the current mouse location. Perhaps we should either
// (a) Disable scaling when not moused over a control point
// (b) Fix the jumping behaviour by calculating motion vectors from the start
//     of the interaction.
void keyinContourWidget::ScaleContourAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);

	if (self->WidgetState != keyinContourWidget::Manipulate)
		return;

	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	double pos[2];
	pos[0] = X;
	pos[1] = Y;

	if (rep->ActivateNode(X, Y))
	{
		self->Superclass::StartInteraction();
		self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
		self->StartInteraction();
		rep->SetCurrentOperationToScale(); // Here
		rep->StartWidgetInteraction(pos);
		self->EventCallbackCommand->SetAbortFlag(1);
	}
	else
	{
		double p[3];
		int idx;
		if (rep->FindClosestPointOnContour(X, Y, p, &idx))
		{
			rep->GetNthNodeDisplayPosition(idx, pos);
			rep->ActivateNode(pos);
			self->Superclass::StartInteraction();
			self->InvokeEvent(vtkCommand::StartInteractionEvent, nullptr);
			self->StartInteraction();
			rep->SetCurrentOperationToScale(); // Here
			rep->StartWidgetInteraction(pos);
			self->EventCallbackCommand->SetAbortFlag(1);
		}
	}

	if (rep->GetNeedToRender())
	{
		self->Render();
		rep->NeedToRenderOff();
	}
}

//-------------------------------------------------------------------------
void keyinContourWidget::DeleteAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);

	if (self->WidgetState == keyinContourWidget::Start)
	{
		return;
	}

	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	if (self->WidgetState == keyinContourWidget::Define)
	{
		if (rep->DeleteLastNode())
		{
			self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
		}
	}
	//else
	//{
	//	int X = self->Interactor->GetEventPosition()[0];
	//	int Y = self->Interactor->GetEventPosition()[1];
	//	rep->ActivateNode(X, Y);
	//	if (rep->DeleteActiveNode())
	//	{
	//		self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	//	}
	//	rep->ActivateNode(X, Y);
	//	int numNodes = rep->GetNumberOfNodes();
	//	if (numNodes < 3)
	//	{
	//		rep->ClosedLoopOff();
	//		if (numNodes < 2)
	//		{
	//			self->WidgetState = keyinContourWidget::Define;
	//		}
	//	}
	//}

	if (rep->GetNeedToRender())
	{
		self->Render();
		rep->NeedToRenderOff();
	}
}

//-------------------------------------------------------------------------
void keyinContourWidget::MoveAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);

	if (self->WidgetState == keyinContourWidget::Start)
	{
		return;
	}

	int X = self->Interactor->GetEventPosition()[0];
	int Y = self->Interactor->GetEventPosition()[1];
	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	if (self->WidgetState == keyinContourWidget::Define)
	{
		if (self->FollowCursor || self->ContinuousDraw)
		{
			// Have the last node follow the mouse in this case...
			const int numNodes = rep->GetNumberOfNodes();

			// First check if the last node is near the first node, if so, we intend
			// closing the loop.
			if (numNodes > 1)
			{
				double displayPos[2];
				int pixelTolerance = rep->GetPixelTolerance();
				int pixelTolerance2 = pixelTolerance * pixelTolerance;

				rep->GetNthNodeDisplayPosition(0, displayPos);

				int distance2 = static_cast<int>((X - displayPos[0]) * (X - displayPos[0]) +
					(Y - displayPos[1]) * (Y - displayPos[1]));

				const bool mustCloseLoop =
					(distance2 < pixelTolerance2 && numNodes > 2) ||
					(self->ContinuousDraw && numNodes > pixelTolerance && distance2 < pixelTolerance2);

				if (mustCloseLoop != (rep->GetClosedLoop() == 1))
				{
					if (rep->GetClosedLoop())
					{
						// We need to open the closed loop.
						// We do this by adding a node at (X,Y). If by chance the point
						// placer says that (X,Y) is invalid, we'll add it at the location
						// of the first control point (which we know is valid).

						if (!rep->AddNodeAtDisplayPosition(X, Y))
						{
							double closedLoopPoint[3];
							rep->GetNthNodeWorldPosition(0, closedLoopPoint);
							rep->AddNodeAtDisplayPosition(closedLoopPoint);
						}
						rep->ClosedLoopOff();
					}
					else
					{
						// We need to close the open loop. Delete the node that's following
						// the mouse cursor and close the loop between the previous node and
						// the first node.
						rep->DeleteLastNode();
						rep->ClosedLoopOn();
					}
				}
				else if (rep->GetClosedLoop() == 0)
				{
					if (self->ContinuousDraw && self->ContinuousActive)
					{
						rep->AddNodeAtDisplayPosition(X, Y);
					}
					else
					{
						// If we aren't changing the loop topology, simply update the position
						// of the latest node to follow the mouse cursor position (X,Y).
						rep->SetNthNodeDisplayPosition(numNodes - 1, X, Y);
					}
				}
			}
		}
		else
		{
			return;
		}
	}

	if (rep->GetCurrentOperation() == keyinContourRepresentation::Inactive)
	{
		rep->ComputeInteractionState(X, Y);
		if (self->WidgetState == keyinContourWidget::Manipulate)
		{
			rep->ActivateNode(-100, -100);
			//return;
		}
		else
		{

			rep->ActivateNode(X, Y);
		}
	}
	else
	{
		//return;
		double pos[2];
		pos[0] = X;
		pos[1] = Y;
		self->WidgetRep->WidgetInteraction(pos);
		self->InvokeEvent(vtkCommand::InteractionEvent, nullptr);
	}
	
	if (self->WidgetRep->GetNeedToRender())
	{
		self->Render();
		self->WidgetRep->NeedToRenderOff();
	}
}

//-------------------------------------------------------------------------
void keyinContourWidget::EndSelectAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);
	keyinContourRepresentation *rep =
		reinterpret_cast<keyinContourRepresentation*>(self->WidgetRep);

	if (self->ContinuousDraw)
	{
		self->ContinuousActive = 0;
	}

	// Do nothing if inactive
	if (rep->GetCurrentOperation() == keyinContourRepresentation::Inactive)
	{
		rep->SetRebuildLocator(true);
		return;
	}

	rep->SetCurrentOperationToInactive();
	self->EventCallbackCommand->SetAbortFlag(1);
	self->Superclass::EndInteraction();
	self->InvokeEvent(vtkCommand::EndInteractionEvent, nullptr);

	// Node picking
	if (self->AllowNodePicking && self->Interactor->GetControlKey() &&
		self->WidgetState == keyinContourWidget::Manipulate)
	{
		rep->ToggleActiveNodeSelected();
	}

	if (self->WidgetRep->GetNeedToRender())
	{
		self->Render();
		self->WidgetRep->NeedToRenderOff();
	}
}

//-------------------------------------------------------------------------
void keyinContourWidget::ResetAction(vtkAbstractWidget *w)
{
	keyinContourWidget *self = reinterpret_cast<keyinContourWidget*>(w);
	self->Initialize(nullptr);
}

//----------------------------------------------------------------------
void keyinContourWidget::Initialize(vtkPolyData * pd, int state, vtkIdList *idList)
{
	if (!this->GetEnabled())
	{
		vtkErrorMacro(<< "Enable widget before initializing");
	}

	if (this->WidgetRep)
	{
		keyinContourRepresentation *rep =
			reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep);

		if (pd == nullptr)
		{
			while (rep->DeleteLastNode())
			{
				;
			}
			rep->ClosedLoopOff();
			this->Render();
			rep->NeedToRenderOff();
			rep->VisibilityOff();
			this->WidgetState = keyinContourWidget::Start;
		}
		else
		{
			rep->Initialize(pd, idList);
			this->WidgetState = (rep->GetClosedLoop() || state == 1) ?
				keyinContourWidget::Manipulate : keyinContourWidget::Define;
		}
	}
}

//----------------------------------------------------------------------
void keyinContourWidget::SetAllowNodePicking(vtkTypeBool val)
{
	if (this->AllowNodePicking == val)
	{
		return;
	}
	this->AllowNodePicking = val;
	if (this->AllowNodePicking)
	{
		keyinContourRepresentation *rep =
			reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep);
		rep->SetShowSelectedNodes(this->AllowNodePicking);
	}
}

//----------------------------------------------------------------------
void keyinContourWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);

	os << indent << "WidgetState: " << this->WidgetState << endl;
	os << indent << "CurrentHandle: " << this->CurrentHandle << endl;
	os << indent << "AllowNodePicking: " << this->AllowNodePicking << endl;
	os << indent << "FollowCursor: " << (this->FollowCursor ? "On" : "Off") << endl;
	os << indent << "ContinuousDraw: " << (this->ContinuousDraw ? "On" : "Off") << endl;
}
