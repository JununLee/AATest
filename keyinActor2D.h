#ifndef KEYINACTOR2D_H
#define KEYINACTOR2D_H

#include <vtkActor2D.h>
#include <vtkImageMapper.h>
#include <vtkProperty2D.h>

class vtkPolyDataMapper2D;
class vtkPolyData;
class vtkTextMapper;
class vtkTextProperty;

class keyinActor2D : public vtkActor2D
{
public:
	vtkTypeMacro(keyinActor2D, vtkActor2D);
	void PrintSelf(ostream& os, vtkIndent indent) override;


	static keyinActor2D *New();

	
	virtual vtkCoordinate *GetPoint1Coordinate()
	{
		return this->GetPositionCoordinate();
	};
	virtual void SetPoint1(double x[2]) { this->SetPosition(x); };
	virtual void SetPoint1(double x, double y) { this->SetPosition(x, y); };
	virtual double *GetPoint1() { return this->GetPosition(); };


	virtual vtkCoordinate *GetPoint2Coordinate()
	{
		return this->GetPosition2Coordinate();
	};
	virtual void SetPoint2(double x[2]) { this->SetPosition2(x); };
	virtual void SetPoint2(double x, double y) { this->SetPosition2(x, y); };
	virtual double *GetPoint2() { return this->GetPosition2(); };


	vtkSetVector2Macro(Range, double);
	vtkGetVectorMacro(Range, double, 2);


	vtkSetMacro(RulerMode, vtkTypeBool);
	vtkGetMacro(RulerMode, vtkTypeBool);
	vtkBooleanMacro(RulerMode, vtkTypeBool);


	vtkSetClampMacro(RulerDistance, double, 0, VTK_FLOAT_MAX);
	vtkGetMacro(RulerDistance, double);


	enum LabelMax
	{
		VTK_MAX_LABELS = 25
	};

	vtkSetClampMacro(NumberOfLabels, int, 2, VTK_MAX_LABELS);
	vtkGetMacro(NumberOfLabels, int);


	vtkSetStringMacro(LabelFormat);
	vtkGetStringMacro(LabelFormat);

	vtkSetMacro(AdjustLabels, vtkTypeBool);
	vtkGetMacro(AdjustLabels, vtkTypeBool);
	vtkBooleanMacro(AdjustLabels, vtkTypeBool);
	virtual double *GetAdjustedRange()
	{
		this->UpdateAdjustedRange();
		return this->AdjustedRange;
	}
	virtual void GetAdjustedRange(double &_arg1, double &_arg2)
	{
		this->UpdateAdjustedRange();
		_arg1 = this->AdjustedRange[0];
		_arg2 = this->AdjustedRange[1];
	};
	virtual void GetAdjustedRange(double _arg[2])
	{
		this->GetAdjustedRange(_arg[0], _arg[1]);
	}
	virtual int GetAdjustedNumberOfLabels()
	{
		this->UpdateAdjustedRange();
		return this->AdjustedNumberOfLabels;
	}

	vtkSetStringMacro(Title);
	vtkGetStringMacro(Title);

	virtual void SetTitleTextProperty(vtkTextProperty *p);
	vtkGetObjectMacro(TitleTextProperty, vtkTextProperty);

	virtual void SetLabelTextProperty(vtkTextProperty *p);
	vtkGetObjectMacro(LabelTextProperty, vtkTextProperty);

	vtkSetClampMacro(TickLength, int, 0, 100);
	vtkGetMacro(TickLength, int);

	vtkSetClampMacro(NumberOfMinorTicks, int, 0, 20);
	vtkGetMacro(NumberOfMinorTicks, int);
	
	vtkSetClampMacro(MinorTickLength, int, 0, 100);
	vtkGetMacro(MinorTickLength, int);
	
	vtkSetClampMacro(TickOffset, int, 0, 100);
	vtkGetMacro(TickOffset, int);

	vtkSetMacro(AxisVisibility, vtkTypeBool);
	vtkGetMacro(AxisVisibility, vtkTypeBool);
	vtkBooleanMacro(AxisVisibility, vtkTypeBool);

	vtkSetMacro(TickVisibility, vtkTypeBool);
	vtkGetMacro(TickVisibility, vtkTypeBool);
	vtkBooleanMacro(TickVisibility, vtkTypeBool);
	
	vtkSetMacro(LabelVisibility, vtkTypeBool);
	vtkGetMacro(LabelVisibility, vtkTypeBool);
	vtkBooleanMacro(LabelVisibility, vtkTypeBool);

	vtkSetMacro(TitleVisibility, vtkTypeBool);
	vtkGetMacro(TitleVisibility, vtkTypeBool);
	vtkBooleanMacro(TitleVisibility, vtkTypeBool);

	vtkSetMacro(TitlePosition, double);
	vtkGetMacro(TitlePosition, double);

	vtkSetClampMacro(FontFactor, double, 0.1, 2.0);
	vtkGetMacro(FontFactor, double);

	vtkSetClampMacro(LabelFactor, double, 0.1, 2.0);
	vtkGetMacro(LabelFactor, double);

	int RenderOverlay(vtkViewport* viewport) override;
	int RenderOpaqueGeometry(vtkViewport* viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport *) override { return 0; }
	
	vtkTypeBool HasTranslucentPolygonalGeometry() override;


	void ReleaseGraphicsResources(vtkWindow *) override;

	static void ComputeRange(double inRange[2],
		double outRange[2],
		int inNumTicks,
		int &outNumTicks,
		double &interval);

	static int SetMultipleFontSize(vtkViewport *viewport,
		vtkTextMapper **textMappers,
		int nbOfMappers,
		int *targetSize,
		double factor,
		int *stringSize);

	vtkSetMacro(SizeFontRelativeToAxis, vtkTypeBool);
	vtkGetMacro(SizeFontRelativeToAxis, vtkTypeBool);
	vtkBooleanMacro(SizeFontRelativeToAxis, vtkTypeBool);

	vtkSetMacro(UseFontSizeFromProperty, vtkTypeBool);
	vtkGetMacro(UseFontSizeFromProperty, vtkTypeBool);
	vtkBooleanMacro(UseFontSizeFromProperty, vtkTypeBool);

	void ShallowCopy(vtkProp *prop) override;

	void SetTextBackImageFile(const char * textImageFile);
	void SetPointImageFile(const char * pointImageFile);

	bool MouseOverLabel(double* p);
	void LabelOffset(double* off);

protected:
	keyinActor2D();
	~keyinActor2D() override;

	vtkTextProperty *TitleTextProperty;
	vtkTextProperty *LabelTextProperty;

	double labelOffset[2];

	char  *Title;
	double Range[2];
	double TitlePosition;
	vtkTypeBool    RulerMode;
	double RulerDistance;
	int   NumberOfLabels;
	char  *LabelFormat;
	vtkTypeBool   AdjustLabels;
	double FontFactor;
	double LabelFactor;
	int   TickLength;
	int   MinorTickLength;
	int   TickOffset;
	int NumberOfMinorTicks;

	double AdjustedRange[2];
	int   AdjustedNumberOfLabels;
	int   NumberOfLabelsBuilt;

	vtkTypeBool   AxisVisibility;
	vtkTypeBool   TickVisibility;
	vtkTypeBool   LabelVisibility;
	vtkTypeBool   TitleVisibility;

	int   LastPosition[2];
	int   LastPosition2[2];

	int   LastSize[2];
	int   LastMaxLabelSize[2];
	int   imageSize[3];
	int   pointSize[3];

	int  SizeFontRelativeToAxis;
	vtkTypeBool UseFontSizeFromProperty;

	virtual void BuildAxis(vtkViewport *viewport);
	static double ComputeStringOffset(double width, double height, double theta);
	static void SetOffsetPosition(double xTick[3], double theta,
		int stringWidth, int stringHeight,
		int offset, vtkActor2D *actor);
	virtual void UpdateAdjustedRange();

	vtkImageMapper *pointMapper;
	vtkActor2D     *pointActor1;
	vtkActor2D     *pointActor2;

	vtkImageMapper *imageMapper;
	vtkActor2D     *imageActor;

	vtkTextMapper *TitleMapper;
	vtkActor2D    *TitleActor;

	vtkTextMapper **LabelMappers;
	vtkActor2D    **LabelActors;

	vtkPolyData         *Axis;
	vtkPolyDataMapper2D *AxisMapper;
	vtkActor2D          *AxisActor;

	vtkTimeStamp  AdjustedRangeBuildTime;
	vtkTimeStamp  BuildTime;

private:
	keyinActor2D(const keyinActor2D&) = delete;
	void operator=(const keyinActor2D&) = delete;
};



#endif // !KEYINACTOR2D_H

