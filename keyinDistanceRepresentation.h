#ifndef KEYINDISTANCEREPRESENTATION_H
#define KEYINDISTANCEREPRESENTATION_H


#include <vtkDistanceRepresentation.h>
#include "keyinActor2D.h"
#include "keyinHandleRepresentation.h"

class vtkAxisActor2D;
class vtkProperty2D;


class keyinDistanceRepresentation : public vtkDistanceRepresentation
{
public:
	static keyinDistanceRepresentation *New();

	vtkTypeMacro(keyinDistanceRepresentation, vtkDistanceRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	double GetDistance() override
	{
		return this->Distance;
	}

	double* GetPoint1WorldPosition() override;
	double* GetPoint2WorldPosition() override;
	void GetPoint1WorldPosition(double pos[3]) override;
	void GetPoint2WorldPosition(double pos[3]) override;
	void SetPoint1WorldPosition(double pos[3]) override;
	void SetPoint2WorldPosition(double pos[3]) override;

	void SetPoint1DisplayPosition(double pos[3]) override;
	void SetPoint2DisplayPosition(double pos[3]) override;
	void GetPoint1DisplayPosition(double pos[3]) override;
	void GetPoint2DisplayPosition(double pos[3]) override;

	void InstantiateHandleRepresentation();

	keyinActor2D *GetAxis();
	vtkProperty2D  *GetAxisProperty();

	void SetTextBackImageFile(const char * textImageFile);
	void SetPointImageFile(const char * pointImageFile);
	void SetHandleImageFile(const char * handleImageFile);

	void BuildRepresentation() override;

	void ReleaseGraphicsResources(vtkWindow *w) override;
	int RenderOverlay(vtkViewport *viewport) override;
	int RenderOpaqueGeometry(vtkViewport *viewport) override;

	vtkGetObjectMacro(Point1Representation, keyinHandleRepresentation);
	vtkGetObjectMacro(Point2Representation, keyinHandleRepresentation);

	int ComputeInteractionState(int X, int Y, int modify = 0) override;
	//void ComplexInteraction(vtkRenderWindowInteractor *iren,vtkAbstractWidget *widget,unsigned long event, void *calldata) override;
	int ComputeComplexInteractionState(vtkRenderWindowInteractor *iren,vtkAbstractWidget *widget,unsigned long event, void *calldata, int modify = 0) override;


protected:
	keyinDistanceRepresentation();
	~keyinDistanceRepresentation() override;

	keyinHandleRepresentation *Point1Representation;
	keyinHandleRepresentation *Point2Representation;

	keyinActor2D   *AxisActor;
	vtkProperty2D  *AxisProperty;

	// The distance between the two points
	double Distance;

private:
	keyinDistanceRepresentation(const keyinDistanceRepresentation&) = delete;
	void operator=(const keyinDistanceRepresentation&) = delete;
};

#endif // !KEYINDISTANCEREPRESENTATION_H

