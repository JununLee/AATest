
#include "keyinLinearContourLineInterpolator.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(keyinLinearContourLineInterpolator);

//----------------------------------------------------------------------
keyinLinearContourLineInterpolator::keyinLinearContourLineInterpolator() = default;

//----------------------------------------------------------------------
keyinLinearContourLineInterpolator::~keyinLinearContourLineInterpolator() = default;

//----------------------------------------------------------------------
int keyinLinearContourLineInterpolator::InterpolateLine(vtkRenderer *vtkNotUsed(ren), keyinContourRepresentation *vtkNotUsed(rep), int vtkNotUsed(idx1), int vtkNotUsed(idx2))
{
	return 1;
}

//----------------------------------------------------------------------
void keyinLinearContourLineInterpolator::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}


