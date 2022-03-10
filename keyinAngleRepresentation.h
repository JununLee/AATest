#ifndef KEYINANGLEREPRESENTATION_H
#define KEYINANGLEREPRESENTATION_H

#include <vtkAngleRepresentation.h>
#include "keyinAngleActor.h"
#include "keyinHandleRepresentation.h"
class vtkLeaderActor2D;
class vtkProperty2D;


class keyinAngleRepresentation : public vtkAngleRepresentation
{
public:
 
	static keyinAngleRepresentation *New();
 
	vtkTypeMacro(keyinAngleRepresentation, vtkAngleRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	double GetAngle() override;
 
	void GetPoint1WorldPosition(double pos[3]) override;
	void GetCenterWorldPosition(double pos[3]) override;
	void GetPoint2WorldPosition(double pos[3]) override;
	void SetPoint1DisplayPosition(double pos[3]) override;
	void SetCenterDisplayPosition(double pos[3]) override;
	void SetPoint2DisplayPosition(double pos[3]) override;
	void GetPoint1DisplayPosition(double pos[3]) override;
	void GetCenterDisplayPosition(double pos[3]) override;
	void GetPoint2DisplayPosition(double pos[3]) override;
 
	void InstantiateHandleRepresentation();

	vtkGetObjectMacro(Point1Representation, keyinHandleRepresentation);
	vtkGetObjectMacro(CenterRepresentation, keyinHandleRepresentation);
	vtkGetObjectMacro(Point2Representation, keyinHandleRepresentation);

	vtkGetObjectMacro(Ray1, keyinAngleActor);
	vtkGetObjectMacro(Ray2, keyinAngleActor);
	vtkGetObjectMacro(Arc,  keyinAngleActor);
 
	void BuildRepresentation() override;

	int ComputeInteractionState(int X, int Y, int modify = 0) override;

	void ReleaseGraphicsResources(vtkWindow *w) override;
	int RenderOverlay(vtkViewport *viewport) override;

	void SetTextBackImageFile(const char * textImageFile);
	void SetPointImageFile(const char * pointImageFile);
	void SetHandleImageFile(const char * handleImageFile);


 
protected:
	keyinAngleRepresentation();
	~keyinAngleRepresentation() override;

	keyinHandleRepresentation *Point1Representation;
	keyinHandleRepresentation *CenterRepresentation;
	keyinHandleRepresentation *Point2Representation;
	// The pieces that make up the angle representations
	keyinAngleActor *Ray1;
	keyinAngleActor *Ray2;
	keyinAngleActor *Arc;

private:
	keyinAngleRepresentation(const keyinAngleRepresentation&) = delete;
	void operator=(const keyinAngleRepresentation&) = delete;
};
#endif // !KEYINANGLEREPRESENTATION_H
