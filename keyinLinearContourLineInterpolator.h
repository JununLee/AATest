#ifndef KEYINLINEARCONTOURLINEINTERPOLATOR_H
#define KEYINLINEARCONTOURLINEINTERPOLATOR_H

#include "keyinContourLineInterpolator.h"

class  keyinLinearContourLineInterpolator: public keyinContourLineInterpolator
{
public:
 
	static keyinLinearContourLineInterpolator *New();
 
	vtkTypeMacro(keyinLinearContourLineInterpolator, keyinContourLineInterpolator);
	void PrintSelf(ostream& os, vtkIndent indent) override;
	 
	int InterpolateLine(vtkRenderer *ren, keyinContourRepresentation *rep, int idx1, int idx2) override;

protected:
	keyinLinearContourLineInterpolator();
	~keyinLinearContourLineInterpolator() override;

private:
	keyinLinearContourLineInterpolator(const keyinLinearContourLineInterpolator&) = delete;
	void operator=(const keyinLinearContourLineInterpolator&) = delete;
};

#endif // !KEYINLINEARCONTOURLINEINTERPOLATOR_H

