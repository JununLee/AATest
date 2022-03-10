#ifndef KEYINCONTOURREPRESENTATION_H
#define KEYINCONTOURREPRESENTATION_H


#include <vtkWidgetRepresentation.h>
#include <vector> // STL Header; Required for vector

class vtkContourLineInterpolator;
class vtkIncrementalOctreePointLocator;
class vtkPointPlacer;
class vtkPolyData;
class vtkIdList;
class keyinContourLineInterpolator;

//----------------------------------------------------------------------
class keyinContourRepresentationPoint
{
public:
	double        WorldPosition[3];
	double        NormalizedDisplayPosition[2];

	// The point id. This is blank except in the case of
	// vtkPolygonalSurfaceContourLineInterpolator
	vtkIdType     PointId;
};

class keyinContourRepresentationNode
{
public:
	double        WorldPosition[3];
	double        WorldOrientation[9];
	double        NormalizedDisplayPosition[2];
	int           Selected;
	std::vector<keyinContourRepresentationPoint*> Points;

	// The point id. This is blank except in the case of
	// vtkPolygonalSurfaceContourLineInterpolator
	vtkIdType     PointId;
};

class keyinContourRepresentationInternals
{
public:
	std::vector<keyinContourRepresentationNode*> Nodes;
	void ClearNodes()
	{
		for (unsigned int i = 0; i < this->Nodes.size(); i++)
		{
			for (unsigned int j = 0; j < this->Nodes[i]->Points.size(); j++)
			{
				delete this->Nodes[i]->Points[j];
			}
			this->Nodes[i]->Points.clear();
			delete this->Nodes[i];
		}
		this->Nodes.clear();
	}
};

class keyinContourRepresentation : public vtkWidgetRepresentation
{
	friend class keyinContourWidget;
public:
	 
	vtkTypeMacro(keyinContourRepresentation, vtkWidgetRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	 
	virtual int AddNodeAtWorldPosition(double x, double y, double z);
	virtual int AddNodeAtWorldPosition(double worldPos[3]);
	virtual int AddNodeAtWorldPosition(double worldPos[3],double worldOrient[9]);
 
	virtual int AddNodeAtDisplayPosition(double displayPos[2]);
	virtual int AddNodeAtDisplayPosition(int displayPos[2]);
	virtual int AddNodeAtDisplayPosition(int X, int Y);
 
	virtual int ActivateNode(double displayPos[2]);
	virtual int ActivateNode(int displayPos[2]);
	virtual int ActivateNode(int X, int Y);
 
	virtual int SetActiveNodeToWorldPosition(double pos[3]);
	virtual int SetActiveNodeToWorldPosition(double pos[3],double orient[9]);
 
	virtual int SetActiveNodeToDisplayPosition(double pos[2]);
	virtual int SetActiveNodeToDisplayPosition(int pos[2]);
	virtual int SetActiveNodeToDisplayPosition(int X, int Y);
	 
	virtual int ToggleActiveNodeSelected();
	virtual int GetActiveNodeSelected();
	virtual int GetNthNodeSelected(int);
	virtual int SetNthNodeSelected(int);
 
	virtual int GetActiveNodeWorldPosition(double pos[3]);
 
	virtual int GetActiveNodeWorldOrientation(double orient[9]);
 
	virtual int GetActiveNodeDisplayPosition(double pos[2]);
 
	virtual int GetNumberOfNodes();
 
	virtual int GetNthNodeDisplayPosition(int n, double pos[2]);
 
	virtual int GetNthNodeWorldPosition(int n, double pos[3]);
 
	virtual keyinContourRepresentationNode *GetNthNode(int n);
 
	virtual int GetNthNodeWorldOrientation(int n, double orient[9]);
 
	virtual int SetNthNodeDisplayPosition(int n, int X, int Y);
	virtual int SetNthNodeDisplayPosition(int n, int pos[2]);
	virtual int SetNthNodeDisplayPosition(int n, double pos[2]);
	 
	virtual int SetNthNodeWorldPosition(int n, double pos[3]);
	virtual int SetNthNodeWorldPosition(int n, double pos[3],double orient[9]);
	 
	virtual int  GetNthNodeSlope(int idx, double slope[3]);
 
	virtual int GetNumberOfIntermediatePoints(int n);
 
	virtual int GetIntermediatePointWorldPosition(int n,int idx, double point[3]);
 
	virtual int AddIntermediatePointWorldPosition(int n,double point[3]);
 
	virtual int AddIntermediatePointWorldPosition(int n,double point[3], vtkIdType ptId);
 
	virtual int DeleteLastNode();
 
	virtual int DeleteActiveNode();
 
	virtual int DeleteNthNode(int n);
 
	virtual void ClearAllNodes();
 
	virtual int AddNodeOnContour(int X, int Y);
 
	vtkSetClampMacro(PixelTolerance, int, 1, 100);
	vtkGetMacro(PixelTolerance, int);
 
	vtkSetClampMacro(WorldTolerance, double, 0.0, VTK_DOUBLE_MAX);
	vtkGetMacro(WorldTolerance, double);
	 
	enum {
		Outside = 0,
		Nearby
	};

	enum {
		Inactive = 0,
		Translate,
		Shift,
		Scale
	};
 
	vtkGetMacro(CurrentOperation, int);
	vtkSetClampMacro(CurrentOperation, int, keyinContourRepresentation::Inactive, keyinContourRepresentation::Scale);
	void SetCurrentOperationToInactive()
	{
		this->SetCurrentOperation(keyinContourRepresentation::Inactive);
	}
	void SetCurrentOperationToTranslate()
	{
		this->SetCurrentOperation(keyinContourRepresentation::Translate);
	}
	void SetCurrentOperationToShift()
	{
		this->SetCurrentOperation(keyinContourRepresentation::Shift);
	}
	void SetCurrentOperationToScale()
	{
		this->SetCurrentOperation(keyinContourRepresentation::Scale);
	}
 
	void SetPointPlacer(vtkPointPlacer *);
	vtkGetObjectMacro(PointPlacer, vtkPointPlacer);
 
	void SetLineInterpolator(keyinContourLineInterpolator *);
	vtkGetObjectMacro(LineInterpolator, keyinContourLineInterpolator);
 
	void BuildRepresentation() override = 0;
	int ComputeInteractionState(int X, int Y, int modified = 0) override = 0;
	void StartWidgetInteraction(double e[2]) override = 0;
	void WidgetInteraction(double e[2]) override = 0;
	 
	void ReleaseGraphicsResources(vtkWindow *w) override = 0;
	int RenderOverlay(vtkViewport *viewport) override = 0;
	int RenderOpaqueGeometry(vtkViewport *viewport) override = 0;
	int RenderTranslucentPolygonalGeometry(vtkViewport *viewport) override = 0;
	vtkTypeBool HasTranslucentPolygonalGeometry() override = 0;
 
	void SetClosedLoop(vtkTypeBool val);
	vtkGetMacro(ClosedLoop, vtkTypeBool);
	vtkBooleanMacro(ClosedLoop, vtkTypeBool);
	 
	virtual void SetShowSelectedNodes(vtkTypeBool);
	vtkGetMacro(ShowSelectedNodes, vtkTypeBool);
	vtkBooleanMacro(ShowSelectedNodes, vtkTypeBool);
 
	virtual vtkPolyData* GetContourRepresentationAsPolyData() = 0;
 
	void GetNodePolyData(vtkPolyData* poly);

	vtkSetMacro(RebuildLocator, bool);

protected:
	keyinContourRepresentation();
	~keyinContourRepresentation() override;

	// Selection tolerance for the handles
	int    PixelTolerance;
	double WorldTolerance;

	vtkPointPlacer             *PointPlacer;
	keyinContourLineInterpolator *LineInterpolator;

	int ActiveNode;

	int CurrentOperation;
	vtkTypeBool ClosedLoop;

	// A flag to indicate whether to show the Selected nodes
	vtkTypeBool                   ShowSelectedNodes;

	keyinContourRepresentationInternals *Internal;

	void AddNodeAtPositionInternal(double worldPos[3], double worldOrient[9], int displayPos[2]);
	void AddNodeAtPositionInternal(double worldPos[3], double worldOrient[9], double displayPos[2]);
	void SetNthNodeWorldPositionInternal(int n, double worldPos[3], double worldOrient[9]);
 
	void GetRendererComputedDisplayPositionFromWorldPosition(double worldPos[3],double worldOrient[9], int displayPos[2]);
	void GetRendererComputedDisplayPositionFromWorldPosition(double worldPos[3],double worldOrient[9], double displayPos[2]);
	 
	virtual void UpdateLines(int index);
	void UpdateLine(int idx1, int idx2);

	virtual int FindClosestPointOnContour(int X, int Y, double worldPos[3], int *idx);

	virtual void BuildLines() = 0;
 
	virtual int  UpdateContour();
	vtkTimeStamp ContourBuildTime;

	void ComputeMidpoint(double p1[3], double p2[3], double mid[3])
	{
		mid[0] = (p1[0] + p2[0]) / 2;
		mid[1] = (p1[1] + p2[1]) / 2;
		mid[2] = (p1[2] + p2[2]) / 2;
	}
 
	virtual void Initialize(vtkPolyData *, vtkIdList *);
 
	virtual void Initialize(vtkPolyData *);
 
	virtual void InitializeContour(vtkPolyData *, vtkIdList *);
 
	vtkIncrementalOctreePointLocator *Locator;
 
	void ResetLocator();

	void BuildLocator();

	bool RebuildLocator;


private:
	keyinContourRepresentation(const keyinContourRepresentation&) = delete;
	void operator=(const keyinContourRepresentation&) = delete;
};

#endif // ! KEYINCONTOURREPRESENTATION_H

