#ifndef KEYINANGLEMEASURE_H
#define KEYINANGLEMEASURE_H

#include "keyinAngleWidget.h"
#include "keyinAngleRepresentation.h"
#include <vtkRenderWindowInteractor.h>

class keyinAngleMeasure
{
public:
	void On(vtkRenderWindowInteractor* interactor);
	void SetLineColor(double x, double y, double z);
	void SetTextColor(double x, double y, double z);
	void SetLineWidth(double w);
	void SetLabelFormat(const char* labelFormat);
	void SetFontSize(int size);
	void SetFontFile(const char* fontFile);

	void SetTextBackImageFile(const char* textImageFile);
	void SetPointImageFile(const char* pointImageFile);
	void SetHandleImageFile(const char* handleImageFile);

	keyinAngleWidget           *angleWidget;
	keyinAngleRepresentation   *angleRep;

	keyinAngleMeasure();
	~keyinAngleMeasure();

private:

};


#endif // !KEYINANGLEMEASURE_H

