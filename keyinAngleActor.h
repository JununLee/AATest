#ifndef KEYINANGLEACTOR_H
#define KEYINANGLEACTOR_H

#include <vtkActor2D.h>
#include <vtkImageMapper.h>

class vtkPoints;
class vtkCellArray;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkTextMapper;
class vtkTextProperty;

class keyinAngleActor : public vtkActor2D
{
public:
	vtkTypeMacro(keyinAngleActor, vtkActor2D);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	static keyinAngleActor *New();
 
	vtkSetMacro(Radius, double);
	vtkGetMacro(Radius, double);
 
	vtkSetStringMacro(Label);
	vtkGetStringMacro(Label);
 
	virtual void SetLabelTextProperty(vtkTextProperty *p);
	vtkGetObjectMacro(LabelTextProperty, vtkTextProperty);
 
	vtkSetClampMacro(LabelFactor, double, 0.1, 2.0);
	vtkGetMacro(LabelFactor, double);
 
	enum { VTK_ARROW_NONE = 0, VTK_ARROW_POINT1, VTK_ARROW_POINT2, VTK_ARROW_BOTH };
	enum { VTK_ARROW_FILLED = 0, VTK_ARROW_OPEN, VTK_ARROW_HOLLOW };
 
	vtkSetClampMacro(ArrowPlacement, int, VTK_ARROW_NONE, VTK_ARROW_BOTH);
	vtkGetMacro(ArrowPlacement, int);
	void SetArrowPlacementToNone() { this->SetArrowPlacement(VTK_ARROW_NONE); }
	void SetArrowPlacementToPoint1() { this->SetArrowPlacement(VTK_ARROW_POINT1); }
	void SetArrowPlacementToPoint2() { this->SetArrowPlacement(VTK_ARROW_POINT2); }
	void SetArrowPlacementToBoth() { this->SetArrowPlacement(VTK_ARROW_BOTH); }
 
	vtkSetClampMacro(ArrowStyle, int, VTK_ARROW_FILLED, VTK_ARROW_HOLLOW);
	vtkGetMacro(ArrowStyle, int);
	void SetArrowStyleToFilled() { this->SetArrowStyle(VTK_ARROW_FILLED); }
	void SetArrowStyleToOpen() { this->SetArrowStyle(VTK_ARROW_OPEN); }
	void SetArrowStyleToHollow() { this->SetArrowStyle(VTK_ARROW_HOLLOW); }
 
	vtkSetClampMacro(ArrowLength, double, 0.0, 1.0);
	vtkGetMacro(ArrowLength, double);
	vtkSetClampMacro(ArrowWidth, double, 0.0, 1.0);
	vtkGetMacro(ArrowWidth, double);
 
	vtkSetClampMacro(MinimumArrowSize, double, 1.0, VTK_FLOAT_MAX);
	vtkGetMacro(MinimumArrowSize, double);
	vtkSetClampMacro(MaximumArrowSize, double, 1.0, VTK_FLOAT_MAX);
	vtkGetMacro(MaximumArrowSize, double);
 
	vtkSetMacro(AutoLabel, vtkTypeBool);
	vtkGetMacro(AutoLabel, vtkTypeBool);
	vtkBooleanMacro(AutoLabel, vtkTypeBool);
 
	vtkSetStringMacro(LabelFormat);
	vtkGetStringMacro(LabelFormat);
 
	vtkGetMacro(Length, double);
	vtkGetMacro(Angle, double);
 
	int RenderOverlay(vtkViewport* viewport) override;
	int RenderOpaqueGeometry(vtkViewport* viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport *) override { return 0; }
 
	vtkTypeBool HasTranslucentPolygonalGeometry() override;

	void ReleaseGraphicsResources(vtkWindow *) override;
	void ShallowCopy(vtkProp *prop) override;

	void SetTextBackImageFile(const char * textImageFile);
	void SetPointImageFile(const char * pointImageFile);

	
	bool MouseOverLabel(double* p);

	double centerPos[3];

protected:
	keyinAngleActor();
	~keyinAngleActor() override;

	// Internal helper methods
	virtual void BuildLeader(vtkViewport *viewport);
	int SetFontSize(vtkViewport *viewport, vtkTextMapper *textMapper,
		int *targetSize, double factor, int *stringSize);
	int ClipLeader(double xL[3], int stringSize[2], double p1[3], double ray[3],
		double c1[3], double c2[3]);
	void BuildCurvedLeader(double p1[3], double p2[3], double ray[3], double rayLength,
		double theta, vtkViewport *viewport, int viewportChanged);
	int InStringBox(double center[3], int stringSize[2], double x[3]);

	vtkCoordinate *Position3Coordinate;

	int   pointSize[3];
	vtkImageMapper *pointMapper;
	vtkActor2D     *pointActor1;
	vtkActor2D     *pointActor2;

	int   imageSize[3];
	vtkImageMapper *imageMapper;
	vtkActor2D     *imageActor;


	// Characteristics of the leader
	double Radius;
	double Length;
	double Angle;

	vtkTypeBool              AutoLabel;
	char            *LabelFormat;
	char            *Label;
	double           LabelFactor;
	vtkTextMapper   *LabelMapper;
	vtkActor2D      *LabelActor;
	vtkTextProperty *LabelTextProperty;

	int    ArrowPlacement;
	int    ArrowStyle;
	double ArrowLength;
	double ArrowWidth;
	double MinimumArrowSize;
	double MaximumArrowSize;

	vtkPoints           *LeaderPoints;
	vtkCellArray        *LeaderLines;
	vtkCellArray        *LeaderArrows;
	vtkPolyData         *Leader;
	vtkPolyDataMapper2D *LeaderMapper;
	vtkActor2D          *LeaderActor;

	// Internal ivars for tracking whether to rebuild
	int LastPosition[2];
	int LastPosition2[2];
	int LastSize[2];
	vtkTimeStamp  BuildTime;


private:
	keyinAngleActor(const keyinAngleActor&) = delete;
	void operator=(const keyinAngleActor&) = delete;
};


#endif // ! KEYINANGLEACTOR_H

