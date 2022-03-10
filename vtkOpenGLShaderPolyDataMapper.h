#pragma once
#ifndef VTKOPENGLSHADERPOLYDATAMAPPER_H
#define VTKOPENGLSHADERPOLYDATAMAPPER_H

#include <vtkOpenGLPolyDataMapper.h>
#include <vtkShaderProgram.h>
#include <vtkOpenGLHelper.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkOpenGLVertexArrayObject.h>
#include <vtkOpenGLShaderCache.h>
#include <vtkCommand.h>
//#include <vtkOpenGLError.h>
#include <vtkOpenGLState.h>
//#include <vtk_glew.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLTexture.h>
#include <vector>
#include <map>
#include <vtkOpenGLVertexBufferObject.h>
#include <vtkOpenGLIndexBufferObject.h>
#include <vtkOpenGLVertexBufferObjectGroup.h>
#include <vtkTextureObject.h>
#include <vtkOpenGLRenderPass.h>
#include <vtkHardwareSelector.h>
#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkVolumeTexture.h>
#include <vtkTextureObject.h>
#include <vtkTimerLog.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>


#define ReadyShaderParama(type,object,name) \
void Set##name(const char* _arg1, type _arg2)\
{\
	object[_arg1] = _arg2;\
}\

#define ReadyShaderParamas(type,object,name,size) \
void Set##name(const char* _arg1, type _arg2)\
{\
	int i;\
	for(i=0;i<size;i++)\
	{\
		object[_arg1][i] = _arg2[i]; \
	}\
}\

#define SetShaderParama(object,name)\
for(auto var : object)\
{\
	shaderProgram->SetUniform##name(var.first.c_str(), var.second);\
}\

class vtkOpenGLState;

typedef std::pair<vtkTexture*, std::string> texinfo;
typedef std::map<std::string, vtkTexture*> Texs;
typedef std::map<std::string, vtkVolumeTexture*> VolumeTexs;
typedef std::map<std::string, float> Vec1s;
typedef std::map<std::string, float*> Vec2s;
typedef std::map<std::string, float[3]> Vec3s;
typedef std::map<std::string, float*> Vec4s;
typedef std::map<std::string, double> Dou1s;
typedef std::map<std::string, const double*> Dou2s;
typedef std::map<std::string, double[3]> Dou3s;
typedef std::map<std::string, const double*> Dou4s;
typedef std::map<std::string, int> Int1s;
typedef std::map<std::string, float*> Mat3x3s;
typedef std::map<std::string, float*> Mat4x4s;
typedef std::map<std::string, vtkMatrix3x3*> vtkMat3x3s;
typedef std::map<std::string, vtkMatrix4x4*> vtkMat4x4s;



class vtkOpenGLShaderPolyDataMapper:public vtkOpenGLPolyDataMapper
{
public:

	bool CullFace = true;
	bool CullFront = false;
	int ZWrite = 1;//GL_TRUE;
	int  DepthFunc = 1;// GL_LEQUAL;
	bool Blend = true;
	bool ColorMask = true;

	static vtkOpenGLShaderPolyDataMapper*New();
	vtkTypeMacro(vtkOpenGLPolyDataMapper, vtkPolyDataMapper)

	void UseShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	void SetVec1(const char* name, float value) {
		vec1[name] = value;
	}
	/*void SetVec1(const char* name, double value) {
		dou1[name] = value;
	}*/
	void SetInt(const char* name, int value) {
		int1[name] = value;
	}

	void SetVec3(const char* name, float* value) {
		vec3[name][0] = value[0];
		vec3[name][1] = value[1];
		vec3[name][2] = value[2];
	}

	ReadyShaderParama(vtkTexture*, textures, Texture);
	ReadyShaderParama(vtkVolumeTexture*, volumeTex, VolumeTexture);
	ReadyShaderParama(float*, vec2, Vec2);
	//ReadyShaderParamas(float*, vec3, Vec3,3);
	ReadyShaderParama(float*, vec4, Vec4);
	//ReadyShaderParama(double*, dou2, Vec2);
	ReadyShaderParamas(double*, dou3, Vec3,3);
	//ReadyShaderParama(double*, dou4, Vec4);
	ReadyShaderParama(float*, mat3x3, Mat3x3);
	ReadyShaderParama(vtkMatrix3x3*, vtkMat3x3, Mat3x3);
	ReadyShaderParama(float*, mat4x4, Mat4x4);
	ReadyShaderParama(vtkMatrix4x4*, vtkMat4x4, Mat4x4);

	virtual void Render(vtkRenderer* ren, vtkActor* act);

	virtual void UpdateShaders(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor);
	virtual void SetMapperShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor);
	virtual void SetPropertyShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor *actor);
	virtual void SetCameraShaderParameters(vtkOpenGLHelper &cellBO, vtkRenderer* ren, vtkActor* actor);

protected:
	vtkOpenGLShaderPolyDataMapper();
	~vtkOpenGLShaderPolyDataMapper() override;

private:

	vtkShaderProgram * shaderProgram;
	vtkOpenGLState* openglState;
	Texs textures;
	VolumeTexs volumeTex;
	Vec1s vec1;
	Vec2s vec2;
	Vec3s vec3;
	Vec4s vec4;
	Dou1s dou1;
	Dou2s dou2;
	Dou3s dou3;
	Dou4s dou4;
	Int1s int1;
	Mat3x3s mat3x3;
	Mat4x4s mat4x4;
	vtkMat3x3s vtkMat3x3;
	vtkMat4x4s vtkMat4x4;

	void SetOpenGLState();
	void ResetOpenGLState();
};

#endif // !VTKOPENGLSHADERPOLYDATAMAPPER_H

