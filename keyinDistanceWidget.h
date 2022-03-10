#ifndef KEYINDISTANCEWIDGET_H
#define KEYINDISTANCEWIDGET_H

#include "vtkAbstractWidget.h"

class vtkDistanceRepresentation;
class vtkHandleWidget;
class vtkDistanceWidgetCallback;


class keyinDistanceWidget : public vtkAbstractWidget
{
public:
	static keyinDistanceWidget *New();
	 
	vtkTypeMacro(keyinDistanceWidget, vtkAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	void SetEnabled(int) override;
 
	void SetRepresentation(vtkDistanceRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(
			reinterpret_cast<vtkWidgetRepresentation*>(r));
	}
 
	vtkDistanceRepresentation *GetDistanceRepresentation()
	{
		return reinterpret_cast<vtkDistanceRepresentation*>(this->WidgetRep);
	}
 
	void CreateDefaultRepresentation() override;
 
	void SetProcessEvents(vtkTypeBool) override;
 

	enum { Start = 0, Define, Manipulate };
 
	virtual void SetWidgetStateToStart();
	virtual void SetWidgetStateToManipulate();
 
	virtual int GetWidgetState()
	{
		return this->WidgetState;
	}

protected:
	keyinDistanceWidget();
	~keyinDistanceWidget() override;

	// The state of the widget
	int WidgetState;
	int CurrentHandle;
	bool moveLabel;
	double lastPos[2];

	// Callback interface to capture events when
	// placing the widget.
	static void AddPointAction(vtkAbstractWidget*);
	static void MoveAction(vtkAbstractWidget*);
	static void EndSelectAction(vtkAbstractWidget*);
	static void AddPointAction3D(vtkAbstractWidget*);
	static void MoveAction3D(vtkAbstractWidget*);
	static void EndSelectAction3D(vtkAbstractWidget*); 
	static void DeleteWidget(vtkAbstractWidget*);

	// The positioning handle widgets
	vtkHandleWidget *Point1Widget;
	vtkHandleWidget *Point2Widget;
	vtkDistanceWidgetCallback *DistanceWidgetCallback1;
	vtkDistanceWidgetCallback *DistanceWidgetCallback2;

	// Methods invoked when the handles at the
	// end points of the widget are manipulated
	void StartDistanceInteraction(int handleNum);
	void DistanceInteraction(int handleNum);
	void EndDistanceInteraction(int handleNum);
	bool MouseOverLabel(keyinDistanceWidget* self);

	friend class vtkDistanceWidgetCallback;

private:
	keyinDistanceWidget(const keyinDistanceWidget&) = delete;
	void operator=(const keyinDistanceWidget&) = delete;
};

#endif // !KEYINDISTANCEWIDGET_H

