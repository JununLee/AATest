#include "keyinContour.h"
#include <vtkProperty.h>

void keyinContour::On(vtkRenderWindowInteractor * interactor)
{
	this->contourWidget->SetInteractor(interactor);
	this->contourWidget->On();
}

void keyinContour::SetLineColor(double x, double y, double z)
{
	this->contourRep->GetLinesProperty()->SetColor(x, y, z);
}

void keyinContour::SetPointColor(double x, double y, double z)
{
	this->contourRep->GetProperty()->SetColor(x, y, z);
}

void keyinContour::SetCompleteCallback(contourCompleteCallBack func)
{
	this->contourWidget->completeCallback = std::bind(func,std::placeholders::_1);
}

keyinContour::keyinContour()
{
	this->contourWidget = keyinContourWidget::New();
	this->contourRep = reinterpret_cast<keyinGlyphContourRepresentation*>(this->contourWidget->GetContourRepresentation());

	this->contourWidget->SetFollowCursor(1);
}

keyinContour::~keyinContour()
{
	this->contourWidget->Delete();
	this->contourRep->Delete();
}
