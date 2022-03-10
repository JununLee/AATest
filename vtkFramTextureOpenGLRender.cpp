#include "vtkFramTextureOpenGLRender.h"
#include <vtkObjectFactory.h>
#include <vtkRenderPass.h>
#include <vtkRenderWindow.h>

vtkStandardNewMacro(vtkFramTextureOpenGLRender);

vtkFramTextureOpenGLRender::vtkFramTextureOpenGLRender()
{
}

vtkFramTextureOpenGLRender::~vtkFramTextureOpenGLRender()
{
}

void vtkFramTextureOpenGLRender::DeviceRender(void)
{
	vtkTimerLog::MarkStartEvent("OpenGL Dev Render");



	if (this->Pass != nullptr)
	{
		vtkRenderState s(this);
		s.SetPropArrayAndCount(this->PropArray, this->PropArrayCount);
		s.SetFrameBuffer(nullptr);
		this->Pass->Render(&s);

	}
	else
	{
		// Do not remove this MakeCurrent! Due to Start / End methods on
		// some objects which get executed during a pipeline update,
		// other windows might get rendered since the last time
		// a MakeCurrent was called.
		this->RenderWindow->MakeCurrent();
		vtkOpenGLClearErrorMacro();


		this->UpdateCamera();
		this->UpdateLightGeometry();
		this->UpdateLights();
		this->UpdateGeometry();


		vtkOpenGLCheckErrorMacro("failed after DeviceRender");
	}

	int* size = this->GetRenderWindow()->GetSize();
	double* viewport = this->GetViewport();
	int oriX = int(*size * viewport[0]);
	int oriY = int(*size * viewport[1]);
	int w = int(*size * (viewport[2] - viewport[0]));
	int h = int(*(size + 1) * (viewport[3] - viewport[1]));
	if (!data)
		data = new unsigned char[w*h * 3];
	glReadPixels(oriX, oriY, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);


	vtkTimerLog::MarkEndEvent("OpenGL Dev Render");
}