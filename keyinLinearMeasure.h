#ifndef KEYINLINEARMEASURE_H
#define KEYINLINEARMEASURE_H

#include "keyinDistanceWidget.h"
#include "keyinDistanceRepresentation.h"
#include <vtkRenderWindowInteractor.h>

class keyinLinearMeasure
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


	keyinDistanceWidget           *lineaWidget;
	keyinDistanceRepresentation *linearRep;

	keyinLinearMeasure();
	~keyinLinearMeasure();

private:

};


#endif // !KEYINLINEARMEASURE_H

