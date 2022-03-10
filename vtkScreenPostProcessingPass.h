#pragma once
#ifndef VTKSCREENPOSTPROCESSINGPASS_H
#define VTKSCREENPOSTPROCESSINGPASS_H

#include <vtkRenderingOpenGL2Module.h>
#include <vtkImageProcessingPass.h>
#include <vtkOpenGLFramebufferObject.h>
#include <vtkOpenGLHelper.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkTextureObject.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include "vtkFramTextureOpenGLRender.h"


class vtkScreenPostProcessingPass : public vtkImageProcessingPass
{
public:
	vtkFramTextureOpenGLRender * renderTex;
	unsigned int id = 0;

	static vtkScreenPostProcessingPass *New(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	//vtkTypeMacro(vtkScreenPostProcessingPass,vtkImageProcessingPass);

	void PrintSelf(ostream& os, vtkIndent indent) override;

	void Render(const vtkRenderState* s) override;

	void ReleaseGraphicsResources(vtkWindow* w) override;




private:
	const char* vertexPath;
	const char* fragmentPath;
	const char* geometryPath;

	vtkOpenGLFramebufferObject* FrameBufferObject;
	vtkTextureObject* Pass;	
	vtkOpenGLHelper* Program;

	vtkShaderProgram* loadShaderProgram(vtkOpenGLRenderWindow* renWin);
	vtkScreenPostProcessingPass(const vtkScreenPostProcessingPass&) = delete;
	void operator=(const vtkScreenPostProcessingPass&) = delete;

	vtkScreenPostProcessingPass(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	~vtkScreenPostProcessingPass() override;
};

#endif // !VTKSCREENPOSTPROCESSINGPASS_H

