#ifndef KEYINHANDLEREPRESENTATION_H
#define KEYINHANDLEREPRESENTATION_H

#include <vtkHandleRepresentation.h>
#include <vtkImageMapper.h>

class vtkProperty2D;
class vtkActor2D;
class vtkCoordinate;
class vtkPolyDataMapper2D;
class vtkPolyData;
class vtkGlyph2D;
class vtkPoints;
class vtkPolyDataAlgorithm;
class vtkPointPlacer;

class keyinHandleRepresentation : public vtkHandleRepresentation
{
public:
	
	static keyinHandleRepresentation *New();

	vtkTypeMacro(keyinHandleRepresentation, vtkHandleRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	void SetCursorShape(vtkPolyData *cursorShape);
	vtkPolyData *GetCursorShape();

	void SetDisplayPosition(double xyz[3]) override;

	void SetProperty(vtkProperty2D*);
	void SetSelectedProperty(vtkProperty2D*);
	vtkGetObjectMacro(Property, vtkProperty2D);
	vtkGetObjectMacro(SelectedProperty, vtkProperty2D);

	double *GetBounds() VTK_SIZEHINT(6) override;
	void BuildRepresentation() override;
	void StartWidgetInteraction(double eventPos[2]) override;
	void WidgetInteraction(double eventPos[2]) override;
	int ComputeInteractionState(int X, int Y, int modify = 0) override;

	void ShallowCopy(vtkProp *prop) override;
	void DeepCopy(vtkProp *prop) override;
	void GetActors2D(vtkPropCollection *) override;
	void ReleaseGraphicsResources(vtkWindow *) override;
	int RenderOverlay(vtkViewport *viewport) override;

	void Highlight(int highlight) override;

	void SetPointPlacer(vtkPointPlacer *) override;

	void SetHandleImageFile(const char * handleImageFile);

protected:
	keyinHandleRepresentation();
	~keyinHandleRepresentation() override;

	int   imageSize[3];
	// Render the cursor
	vtkActor2D           *Actor;
	vtkCoordinate        *MapperCoordinate;
	vtkPolyDataMapper2D  *Mapper;
	vtkImageMapper       *pointMapper;
	vtkGlyph2D           *Glypher;
	vtkPolyData          *CursorShape;
	vtkPolyData          *FocalData;
	vtkPoints            *FocalPoint;

	// Support picking
	double LastPickPosition[3];
	double LastEventPosition[2];

	// Methods to manipulate the cursor
	int  ConstraintAxis;
	void Translate(double eventPos[2]);
	void Scale(double eventPos[2]);

	// Properties used to control the appearance of selected objects and
	// the manipulator in general.
	vtkProperty2D *Property;
	vtkProperty2D *SelectedProperty;
	void           CreateDefaultProperties();

	// The size of the hot spot.
	int    DetermineConstraintAxis(int constraint, double eventPos[2]);
	int    WaitingForMotion;
	int    WaitCount;

private:
	keyinHandleRepresentation(const keyinHandleRepresentation&) = delete;
	void operator=(const keyinHandleRepresentation&) = delete;
};

#endif // !KEYINHANDLEREPRESENTATION_H

