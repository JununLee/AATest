#pragma once
#ifndef VTKFRAMTEXTUREOPENGLRENDER_H
#define VTKFRAMTEXTUREOPENGLRENDER_H

#include <vtkOpenGLRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderState.h>
#include <vtkTimerLog.h>
#include <vtkOpenGLError.h>

class vtkFramTextureOpenGLRender :public vtkOpenGLRenderer
{
public:
	unsigned char* data;

	static vtkFramTextureOpenGLRender *New();
	vtkTypeMacro(vtkFramTextureOpenGLRender, vtkOpenGLRenderer);
	virtual void DeviceRender(void);

protected:
	vtkFramTextureOpenGLRender();
	~vtkFramTextureOpenGLRender() override;

private:

};



#endif // !VTKFRAMTEXTUREOPENGLRENDER_H

