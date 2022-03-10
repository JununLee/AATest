#ifndef KEYINCONTOURWIDGET_H
#define KEYINCONTOURWIDGET_H

#include <vtkAbstractWidget.h>
#include "keyinContourRepresentation.h"
#include <functional>

class vtkPolyData;
class vtkIdList;

typedef std::function<void(keyinContourRepresentation*)> contourCompleteCallBack;
typedef std::function<void()> contourStartCallBack;

class keyinContourWidget : public vtkAbstractWidget
{
public:
	 
	static keyinContourWidget *New();
 
	vtkTypeMacro(keyinContourWidget, vtkAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	 
	void SetEnabled(int) override;
	 
	void SetRepresentation(keyinContourRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
	}
	 
	keyinContourRepresentation *GetContourRepresentation()
	{
		return reinterpret_cast<keyinContourRepresentation*>(this->WidgetRep);
	}
 
	void CreateDefaultRepresentation() override;
 
	void CloseLoop();
	 
	vtkSetMacro(WidgetState, int);
 
	vtkGetMacro(WidgetState, int);
 
	void SetAllowNodePicking(vtkTypeBool);
	vtkGetMacro(AllowNodePicking, vtkTypeBool);
	vtkBooleanMacro(AllowNodePicking, vtkTypeBool);
 
	vtkSetMacro(FollowCursor, vtkTypeBool);
	vtkGetMacro(FollowCursor, vtkTypeBool);
	vtkBooleanMacro(FollowCursor, vtkTypeBool);
 
	vtkSetMacro(ContinuousDraw, vtkTypeBool);
	vtkGetMacro(ContinuousDraw, vtkTypeBool);
	vtkBooleanMacro(ContinuousDraw, vtkTypeBool);
 
	virtual void Initialize(vtkPolyData * poly, int state = 1, vtkIdList *idList = nullptr);
	virtual void Initialize()
	{
		this->Initialize(nullptr);
	}

	// The state of the widget

	enum { Start, Define, Manipulate };

	contourStartCallBack startCallback;
	contourCompleteCallBack completeCallback;

protected:
	keyinContourWidget();
	~keyinContourWidget() override;

	int WidgetState;
	int CurrentHandle;
	vtkTypeBool AllowNodePicking;
	vtkTypeBool FollowCursor;
	vtkTypeBool ContinuousDraw;
	int ContinuousActive;

	// Callback interface to capture events when
	// placing the widget.
	static void SelectAction(vtkAbstractWidget*);
	static void AddFinalPointAction(vtkAbstractWidget*);
	static void MoveAction(vtkAbstractWidget*);
	static void EndSelectAction(vtkAbstractWidget*);
	static void DeleteAction(vtkAbstractWidget*);
	static void TranslateContourAction(vtkAbstractWidget*);
	static void ScaleContourAction(vtkAbstractWidget*);
	static void ResetAction(vtkAbstractWidget*);

	// Internal helper methods
	void SelectNode();
	void AddNode();

private:
	keyinContourWidget(const keyinContourWidget&) = delete;
	void operator=(const keyinContourWidget&) = delete;
};

#endif // !KEYINCONTOURWIDGET_H

