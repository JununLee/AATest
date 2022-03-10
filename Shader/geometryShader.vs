#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
 
 out VS_OUT{
	vec2 texcoord;
 }vs_out;
 
 layout (std140) uniform Matrices{
	mat4 view;
	mat4 projection;
 } ;
 
 uniform mat4 model;
 
void main(){
	vs_out.texcoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos,1);
}