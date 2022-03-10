#ifndef KEYINANGLEWIDGET_H
#define KEYINANGLEWIDGET_H

#include "vtkAbstractWidget.h"

class vtkAngleRepresentation;
class vtkHandleWidget;
class vtkAngleWidgetCallback;


class keyinAngleWidget : public vtkAbstractWidget
{
public:
	 
	static keyinAngleWidget *New();
 
	vtkTypeMacro(keyinAngleWidget, vtkAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	void SetEnabled(int) override;
 
	void SetRepresentation(vtkAngleRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
	}
 
	void CreateDefaultRepresentation() override;
 
	vtkAngleRepresentation *GetAngleRepresentation()
	{
		return reinterpret_cast<vtkAngleRepresentation*>(this->WidgetRep);
	}
 
	vtkTypeBool IsAngleValid();
 
	void SetProcessEvents(vtkTypeBool) override;
 
	enum { Start = 0, Define, Manipulate };
 
	virtual void SetWidgetStateToStart();
	virtual void SetWidgetStateToManipulate();
	 
	virtual int GetWidgetState()
	{
		return this->WidgetState;
	}

protected:
	keyinAngleWidget();
	~keyinAngleWidget() override;

	// The state of the widget
	int WidgetState;
	int CurrentHandle;

	// Callback interface to capture events when
	// placing the widget.
	static void AddPointAction(vtkAbstractWidget*);
	static void MoveAction(vtkAbstractWidget*);
	static void EndSelectAction(vtkAbstractWidget*);
	static void DeleteWidget(vtkAbstractWidget*);

	// The positioning handle widgets
	vtkHandleWidget *Point1Widget;
	vtkHandleWidget *CenterWidget;
	vtkHandleWidget *Point2Widget;
	vtkAngleWidgetCallback *AngleWidgetCallback1;
	vtkAngleWidgetCallback *AngleWidgetCenterCallback;
	vtkAngleWidgetCallback *AngleWidgetCallback2;

	// Methods invoked when the handles at the
	// end points of the widget are manipulated
	void StartAngleInteraction(int handleNum);
	void AngleInteraction(int handleNum);
	void EndAngleInteraction(int handleNum);

	friend class vtkAngleWidgetCallback;

private:
	keyinAngleWidget(const keyinAngleWidget&) = delete;
	void operator=(const keyinAngleWidget&) = delete;
};

#endif // ! KEYINANGLEWIDGET_H

