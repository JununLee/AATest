#ifndef MYTESTREPRESENTATION_H
#define MYTESTREPRESENTATION_H

#include <vtkBiDimensionalRepresentation.h>

class vtkHandleRepresentation;
class vtkCellArray;
class vtkPoints;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkTextMapper;
class vtkActor2D;
class vtkProperty2D;
class vtkTextProperty;


class myTestRepresentation : public vtkBiDimensionalRepresentation
{
public:
	/**
	 * Instantiate the class.
	 */
	static myTestRepresentation *New();

	//@{
	/**
	 * Standard VTK methods.
	 */
	vtkTypeMacro(myTestRepresentation, vtkBiDimensionalRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	//@}

	//@{
	/**
	 * Retrieve the property used to control the appearance of the two
	 * orthogonal lines.
	 */
	vtkGetObjectMacro(LineProperty, vtkProperty2D);
	vtkGetObjectMacro(SelectedLineProperty, vtkProperty2D);
	//@}

	//@{
	/**
	 * Retrieve the property used to control the appearance of the text
	 * labels.
	 */
	vtkGetObjectMacro(TextProperty, vtkTextProperty);
	//@}

	// Used to communicate about the state of the representation
	enum { Outside = 0, NearP1, NearP2, NearP3, NearP4, OnL1Inner, OnL1Outer, OnL2Inner, OnL2Outer, OnCenter };

	//@{
	/**
	 * These are methods that satisfy vtkWidgetRepresentation's API.
	 */
	void BuildRepresentation() override;
	int ComputeInteractionState(int X, int Y, int modify = 0) override;
	void StartWidgetDefinition(double e[2]) override;
	void Point2WidgetInteraction(double e[2]) override;
	void Point3WidgetInteraction(double e[2]) override;
	void StartWidgetManipulation(double e[2]) override;
	void WidgetInteraction(double e[2]) override;
	void Highlight(int highlightOn) override;
	//@}

	//@{
	/**
	 * Methods required by vtkProp superclass.
	 */
	void ReleaseGraphicsResources(vtkWindow *w) override;
	int RenderOverlay(vtkViewport *viewport) override;
	//@}

	/**
	 * Get the text shown in the widget's label.
	 */
	char* GetLabelText() override;

	//@{
	/**
	 * Get the position of the widget's label in display coordinates.
	 */
	double* GetLabelPosition() override;
	void GetLabelPosition(double pos[3]) override;
	void GetWorldLabelPosition(double pos[3]) override;
	//@}
	vtkTextMapper   *TextMapper;
protected:
	myTestRepresentation();
	~myTestRepresentation() override;

	// Geometry of the lines
	vtkCellArray        *LineCells;
	vtkPoints           *LinePoints;
	vtkPolyData         *LinePolyData;
	vtkPolyDataMapper2D *LineMapper;
	vtkActor2D          *LineActor;
	vtkProperty2D       *LineProperty;
	vtkProperty2D       *SelectedLineProperty;

	// The labels for the line lengths
	vtkTextProperty *TextProperty;
	
	vtkActor2D      *TextActor;

	// Helper method
	void ProjectOrthogonalPoint(double x[4], double y[3], double x1[3], double x2[3], double x21[3],
		double dir, double xP[3]);

private:
	myTestRepresentation(const myTestRepresentation&) = delete;
	void operator=(const myTestRepresentation&) = delete;
};
#endif // !MYTESTREPRESENTATION_H

