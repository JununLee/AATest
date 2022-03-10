#ifndef KEYINCONTOURLINEINTERPOLATOR_H
#define KEYINCONTOURLINEINTERPOLATOR_H

#include <vtkObject.h>
#include "keyinContourRepresentation.h"

class vtkRenderer;
class vtkContourRepresentation;
class vtkIntArray;

class keyinContourLineInterpolator : public vtkObject
{
public:
 
	vtkTypeMacro(keyinContourLineInterpolator, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent) override;
 
	virtual int InterpolateLine(vtkRenderer *ren, keyinContourRepresentation *rep, int idx1, int idx2) = 0;
 
	virtual int UpdateNode(vtkRenderer *, keyinContourRepresentation *, double * vtkNotUsed(node), int vtkNotUsed(idx));
 
	virtual void GetSpan(int nodeIndex, vtkIntArray *nodeIndices, keyinContourRepresentation *rep);

protected:
	keyinContourLineInterpolator();
	~keyinContourLineInterpolator() override;

private:
	keyinContourLineInterpolator(const keyinContourLineInterpolator&) = delete;
	void operator=(const keyinContourLineInterpolator&) = delete;
};

#endif // !KEYINCONTOURLINEINTERPOLATOR_H

