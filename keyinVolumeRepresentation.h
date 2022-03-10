#ifndef KEYINVOLUMEREPRESENTATION_H
#define KEYINVOLUMEREPRESENTATION_H

#include <vtkDistanceRepresentation.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <vtkImageMapper.h>
#include <vtkImageData.h>

class keyinVolumeRepresentation : public vtkDistanceRepresentation
{
public:
	static keyinVolumeRepresentation *New();

	vtkTypeMacro(keyinVolumeRepresentation, vtkDistanceRepresentation);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	double GetDistance() override
	{
		return 0;
	}
	double* GetPoint1WorldPosition() override;
	double* GetPoint2WorldPosition() override;
	void GetPoint1WorldPosition(double pos[3]) override;
	void GetPoint2WorldPosition(double pos[3]) override;
	void SetPoint1WorldPosition(double pos[3]) override;
	void SetPoint2WorldPosition(double pos[3]) override;

	void SetPoint1DisplayPosition(double pos[3]) override;
	void SetPoint2DisplayPosition(double pos[3]) override;
	void GetPoint1DisplayPosition(double pos[3]) override;
	void GetPoint2DisplayPosition(double pos[3]) override;

	vtkSetStringMacro(LabelFormat);

	vtkGetObjectMacro(LabelTextProperty, vtkTextProperty);
	vtkGetObjectMacro(VolumeProperty, vtkProperty);
	vtkGetObjectMacro(VolumeActor, vtkActor);

	void BuildRepresentation() override;

	void ReleaseGraphicsResources(vtkWindow *w) override;
	int RenderOverlay(vtkViewport *viewport) override;
	int RenderOpaqueGeometry(vtkViewport *viewport) override;

	int ComputeInteractionState(int X, int Y, int modify = 0) override;
	int ComputeComplexInteractionState(vtkRenderWindowInteractor *iren, vtkAbstractWidget *widget, unsigned long event, void *calldata, int modify = 0) override;

	void SetTextBackImageFile(const char * textImageFile);

	void volumeCalculate(vtkImageData* input, int* seed, int lower, int upper);

protected:
	keyinVolumeRepresentation();
	~keyinVolumeRepresentation() override;

	vtkProperty       *VolumeProperty;
	vtkPolyDataMapper *VolumeMapper;
	vtkActor          *VolumeActor;

	vtkTextProperty *LabelTextProperty;
	vtkTextMapper   *LabelMapper;
	vtkActor2D      *LabelActor;

	int   imageSize[3];
	vtkImageMapper *imageMapper;
	vtkActor2D     *imageActor;

	char  *LabelFormat;
	double Volume;



private:
	keyinVolumeRepresentation(const keyinVolumeRepresentation&) = delete;
	void operator=(const keyinVolumeRepresentation&) = delete;
};

#endif // !KEYINVOLUMEREPRESENTATION_H

