#version 420 core

layout (location = 0) in vec3 aPos;
 
out vec3 texcoord;

layout (std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

void main(){
	gl_Position =vec4(projection * mat4(mat3(view)) * vec4(aPos,1)).xyww;
	texcoord = aPos;
}