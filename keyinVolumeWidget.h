#ifndef KEYINVOLUMEWIDGET_H
#define KEYINVOLUMEWIDGET_H

#include <vtkAbstractWidget.h>
#include "keyinVolumeRepresentation.h"


class keyinVolumeWidget : public vtkAbstractWidget
{
public:
 
	static keyinVolumeWidget *New();
 
	vtkTypeMacro(keyinVolumeWidget, vtkAbstractWidget);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	void SetEnabled(int) override;
 
	void SetRepresentation(keyinVolumeRepresentation *r)
	{
		this->Superclass::SetWidgetRepresentation(
			reinterpret_cast<vtkWidgetRepresentation*>(r));
	}
 
	keyinVolumeRepresentation *GetDistanceRepresentation()
	{
		return reinterpret_cast<keyinVolumeRepresentation*>(this->WidgetRep);
	}
 
	void CreateDefaultRepresentation() override;
 
	void SetProcessEvents(vtkTypeBool) override;
 
 
	virtual void SetWidgetStateToStart();
	virtual void SetWidgetStateToManipulate();

protected:
	keyinVolumeWidget();
	~keyinVolumeWidget() override;

private:
	keyinVolumeWidget(const keyinVolumeWidget&) = delete;
	void operator=(const keyinVolumeWidget&) = delete;
};


#endif // !KEYINVOLUMEWIDGET_H

