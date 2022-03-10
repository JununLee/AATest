#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 texCoord;
out vec3 worldPos;
out mat3 TBN;

uniform mat4 model;

layout (std140) uniform Matrices{
	mat4 view;
	mat4 projection;
};

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1);
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * tangent);
	vec3 B = normalize(normalMatrix * bitangent);
	vec3 N = normalize(normalMatrix * aNormal);
	TBN = mat3(T,B,N);
	worldPos = (model * vec4(aPos, 1)).xyz;
	texCoord = aTexCoord;
	
}