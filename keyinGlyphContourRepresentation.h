#ifndef KEYINGLYPHCONTOURREPRESENTATION_H
#define KEYINGLYPHCONTOURREPRESENTATION_H

#include "keyinContourRepresentation.h"

class vtkProperty;
class vtkActor;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkGlyph3D;
class vtkPoints;

class keyinGlyphContourRepresentation : public keyinContourRepresentation
{
public:
 
	static keyinGlyphContourRepresentation *New();
	 
	vtkTypeMacro(keyinGlyphContourRepresentation, keyinContourRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	void SetCursorShape(vtkPolyData *cursorShape);
	vtkPolyData *GetCursorShape();
 
	void SetActiveCursorShape(vtkPolyData *activeShape);
	vtkPolyData *GetActiveCursorShape();
 
	vtkGetObjectMacro(Property, vtkProperty);
 
	vtkGetObjectMacro(ActiveProperty, vtkProperty);
	vtkGetObjectMacro(ActiveActor, vtkActor);
	 
	vtkGetObjectMacro(LinesProperty, vtkProperty);
	 
	void SetRenderer(vtkRenderer *ren) override;
	void BuildRepresentation() override;
	void StartWidgetInteraction(double eventPos[2]) override;
	void WidgetInteraction(double eventPos[2]) override;
	int ComputeInteractionState(int X, int Y, int modified = 0) override;
 
	void GetActors(vtkPropCollection *) override;
	void ReleaseGraphicsResources(vtkWindow *) override;
	int RenderOverlay(vtkViewport *viewport) override;
	int RenderOpaqueGeometry(vtkViewport *viewport) override;
	int RenderTranslucentPolygonalGeometry(vtkViewport *viewport) override;
	vtkTypeBool HasTranslucentPolygonalGeometry() override;
 
	vtkPolyData * GetContourRepresentationAsPolyData() override;
 
	vtkSetMacro(AlwaysOnTop, vtkTypeBool);
	vtkGetMacro(AlwaysOnTop, vtkTypeBool);
	vtkBooleanMacro(AlwaysOnTop, vtkTypeBool);
 
	void SetLineColor(double r, double g, double b);
 
	void SetShowSelectedNodes(vtkTypeBool) override;
 
	double *GetBounds() override;


protected:
	keyinGlyphContourRepresentation();
	~keyinGlyphContourRepresentation() override;

	// Render the cursor
	vtkActor             *Actor;
	vtkPolyDataMapper    *Mapper;
	vtkGlyph3D           *Glypher;
	vtkActor             *ActiveActor;
	vtkPolyDataMapper    *ActiveMapper;
	vtkGlyph3D           *ActiveGlypher;
	vtkPolyData          *CursorShape;
	vtkPolyData          *ActiveCursorShape;
	vtkPolyData          *FocalData;
	vtkPoints            *FocalPoint;
	vtkPolyData          *ActiveFocalData;
	vtkPoints            *ActiveFocalPoint;

	vtkPolyData          *SelectedNodesData;
	vtkPoints            *SelectedNodesPoints;
	vtkActor             *SelectedNodesActor;
	vtkPolyDataMapper    *SelectedNodesMapper;
	vtkGlyph3D           *SelectedNodesGlypher;
	vtkPolyData          *SelectedNodesCursorShape;
	void CreateSelectedNodesRepresentation();

	vtkPolyData          *Lines;
	vtkPolyDataMapper    *LinesMapper;
	vtkActor             *LinesActor;

	// Support picking
	double LastPickPosition[3];
	double LastEventPosition[2];

	// Methods to manipulate the cursor
	void Translate(double eventPos[2]);
	void Scale(double eventPos[2]);
	void ShiftContour(double eventPos[2]);
	void ScaleContour(double eventPos[2]);

	void ComputeCentroid(double* ioCentroid);

	// Properties used to control the appearance of selected objects and
	// the manipulator in general.
	vtkProperty   *Property;
	vtkProperty   *ActiveProperty;
	vtkProperty   *LinesProperty;
	void           CreateDefaultProperties();


	// Distance between where the mouse event happens and where the
	// widget is focused - maintain this distance during interaction.
	double InteractionOffset[2];

	vtkTypeBool AlwaysOnTop;

	void BuildLines() override;

private:
	keyinGlyphContourRepresentation(const keyinGlyphContourRepresentation&) = delete;
	void operator=(const keyinGlyphContourRepresentation&) = delete;
};


#endif // !KEYINGLYPHCONTOURREPRESENTATION_H

