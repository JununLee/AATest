
#include "keyinVolumeWidget.h"
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

vtkStandardNewMacro(keyinVolumeWidget);

//----------------------------------------------------------------------
keyinVolumeWidget::keyinVolumeWidget()
{
	this->ManagesCursor = 0;

}

//----------------------------------------------------------------------
keyinVolumeWidget::~keyinVolumeWidget()
{
}

//----------------------------------------------------------------------
void keyinVolumeWidget::CreateDefaultRepresentation()
{
	if (!this->WidgetRep)
	{
		this->WidgetRep = keyinVolumeRepresentation::New();
	}
}

//----------------------------------------------------------------------
void keyinVolumeWidget::SetEnabled(int enabling)
{
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


		this->InvokeEvent(vtkCommand::DisableEvent, nullptr);
		this->SetCurrentRenderer(nullptr);
	}

	// Should only render if there is no parent
	if (this->Interactor && !this->Parent)
	{
		this->Interactor->Render();
	}
}

//----------------------------------------------------------------------
void keyinVolumeWidget::SetProcessEvents(vtkTypeBool pe)
{
	this->Superclass::SetProcessEvents(pe);
}

//----------------------------------------------------------------------
void keyinVolumeWidget::SetWidgetStateToStart()
{
	this->ReleaseFocus();
	this->GetRepresentation()->BuildRepresentation(); // update this->Distance
	this->SetEnabled(this->GetEnabled()); // show/hide the handles properly
}

//----------------------------------------------------------------------
void keyinVolumeWidget::SetWidgetStateToManipulate()
{
	this->ReleaseFocus();
	this->GetRepresentation()->BuildRepresentation(); // update this->Distance
	this->SetEnabled(this->GetEnabled()); // show/hide the handles properly
}

//----------------------------------------------------------------------
void keyinVolumeWidget::PrintSelf(ostream& os, vtkIndent indent)
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os, indent);
}
