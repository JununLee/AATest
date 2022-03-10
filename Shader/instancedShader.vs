#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 texcoord;

layout (std140) uniform Matrices{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;

void main(){
	texcoord = aTexCoord;
	gl_Position = projection * view * instanceMatrix * vec4(aPos,1);
}