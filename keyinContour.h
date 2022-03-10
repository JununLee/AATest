#ifndef KEYINCONTOUR_H
#define KEYINCONTOUR_H

#include "keyinContourWidget.h"
#include "keyinGlyphContourRepresentation.h"

class keyinContour
{
public:

	void On(vtkRenderWindowInteractor* interactor);
	void SetLineColor(double x, double y, double z);
	void SetPointColor(double x, double y, double z);
	void SetCompleteCallback(contourCompleteCallBack func);

	keyinContourWidget         *contourWidget;
	keyinGlyphContourRepresentation *contourRep;

	keyinContour();
	~keyinContour();

private:

};


#endif // !KEYINCONTOUR_H

