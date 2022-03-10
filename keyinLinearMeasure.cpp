#include "keyinLinearMeasure.h"
#include "keyinActor2D.h"
#include <vtkTextProperty.h>


void keyinLinearMeasure::On(vtkRenderWindowInteractor * interactor)
{
	this->lineaWidget->SetInteractor(interactor);
	this->lineaWidget->On();
}

void keyinLinearMeasure::SetLineColor(double x, double y, double z)
{
	this->linearRep->GetAxisProperty()->SetColor(x, y, z);
}

void keyinLinearMeasure::SetTextColor(double x, double y, double z)
{
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetColor(x, y, z);
}

void keyinLinearMeasure::SetLineWidth(double w)
{
	this->linearRep->GetAxisProperty()->SetLineWidth(w);
}

void keyinLinearMeasure::SetLabelFormat(const char * labelFormat)
{
	this->linearRep->SetLabelFormat(labelFormat);
}

void keyinLinearMeasure::SetFontSize(int size)
{
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetFontSize(size);
}

void keyinLinearMeasure::SetFontFile(const char * fontFile)
{
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetFontFamily(VTK_FONT_FILE);
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetFontFile(fontFile);
}

void keyinLinearMeasure::SetTextBackImageFile(const char * textImageFile)
{
	this->linearRep->SetTextBackImageFile(textImageFile);
}

void keyinLinearMeasure::SetPointImageFile(const char * pointImageFile)
{
	this->linearRep->SetPointImageFile(pointImageFile);
}

void keyinLinearMeasure::SetHandleImageFile(const char * handleImageFile)
{
	this->linearRep->SetHandleImageFile(handleImageFile);
}

keyinLinearMeasure::keyinLinearMeasure()
{
	this->lineaWidget = keyinDistanceWidget::New();
	this->linearRep = keyinDistanceRepresentation::New();

	this->linearRep->GetAxisProperty()->SetColor(1, 1, 1);
	this->linearRep->GetAxisProperty()->SetLineWidth(1);

	this->linearRep->SetLabelFormat("%.1fmm");
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetFontSize(12);
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetColor(0, 0, 0);
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetJustificationToCentered();
	this->linearRep->GetAxis()->GetTitleTextProperty()->SetVerticalJustificationToCentered();

	this->lineaWidget->SetRepresentation(linearRep);
}

keyinLinearMeasure::~keyinLinearMeasure()
{
	this->lineaWidget->Delete();
	this->linearRep->Delete();
}
