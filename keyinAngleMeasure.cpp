#include "keyinAngleMeasure.h"
#include "keyinAngleActor.h"
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>

void keyinAngleMeasure::On(vtkRenderWindowInteractor * interactor)
{
	this->angleWidget->SetInteractor(interactor);
	this->angleWidget->On();
}

void keyinAngleMeasure::SetLineColor(double x, double y, double z)
{
	this->angleRep->GetRay1()->GetProperty()->SetColor(x, y, z);
	this->angleRep->GetRay2()->GetProperty()->SetColor(x, y, z);
	this->angleRep->GetArc()->GetProperty()->SetColor(x, y, z);
}

void keyinAngleMeasure::SetTextColor(double x, double y, double z)
{
	this->angleRep->GetArc()->GetLabelTextProperty()->SetColor(x, y, z);
}

void keyinAngleMeasure::SetLineWidth(double w)
{
	this->angleRep->GetRay1()->GetProperty()->SetLineWidth(w);
	this->angleRep->GetRay2()->GetProperty()->SetLineWidth(w);
	this->angleRep->GetArc()->GetProperty()->SetLineWidth(w);
}

void keyinAngleMeasure::SetLabelFormat(const char * labelFormat)
{
	this->angleRep->SetLabelFormat(labelFormat);
}

void keyinAngleMeasure::SetFontSize(int size)
{
	this->angleRep->GetArc()->GetLabelTextProperty()->SetFontSize(size);
}

void keyinAngleMeasure::SetFontFile(const char * fontFile)
{
	this->angleRep->GetArc()->GetLabelTextProperty()->SetFontFamily(VTK_FONT_FILE);
	this->angleRep->GetArc()->GetLabelTextProperty()->SetFontFile(fontFile);
}

void keyinAngleMeasure::SetTextBackImageFile(const char * textImageFile)
{
	this->angleRep->SetTextBackImageFile(textImageFile);
}

void keyinAngleMeasure::SetPointImageFile(const char * pointImageFile)
{
	this->angleRep->SetPointImageFile(pointImageFile);
}

void keyinAngleMeasure::SetHandleImageFile(const char * handleImageFile)
{
	this->angleRep->SetHandleImageFile(handleImageFile);
}

keyinAngleMeasure::keyinAngleMeasure()
{
	this->angleWidget = keyinAngleWidget::New();
	this->angleRep = keyinAngleRepresentation::New();

	this->angleRep->GetRay1()->GetProperty()->SetColor(1, 1, 1);
	this->angleRep->GetRay1()->GetProperty()->SetLineWidth(1);
	this->angleRep->GetRay2()->GetProperty()->SetColor(1, 1, 1);
	this->angleRep->GetRay2()->GetProperty()->SetLineWidth(1);
	this->angleRep->GetArc()->GetProperty()->SetColor(1, 1, 1);
	this->angleRep->GetArc()->GetProperty()->SetLineWidth(1);
	this->angleRep->SetLabelFormat(u8"%.1f¡ã");
	this->angleRep->GetArc()->GetLabelTextProperty()->SetFontSize(12);
	this->angleRep->GetArc()->GetLabelTextProperty()->SetColor(0, 0, 0);

	this->angleWidget->SetRepresentation(this->angleRep);
}

keyinAngleMeasure::~keyinAngleMeasure()
{
	this->angleWidget->Delete();
	this->angleRep->Delete();
}
