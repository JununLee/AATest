#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 normal;
out vec3 worldPos;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

layout (std140) uniform Matrices{
	mat4 view;
	mat4 projection;
};

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1);
	normal = aNormal;
	worldPos = (model * vec4(aPos, 1)).xyz;
	texCoord = aTexCoord;
	FragPosLightSpace = lightSpaceMatrix * vec4(worldPos,1);
}