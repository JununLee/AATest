#version 450 core
in vec4 vertexMC;
in vec2 tcoord;
out vec2 TCoord;
out vec4 worldPos;
uniform mat4 MCDCMatrix;
uniform mat4 MCWCMatrix;
void main(){
	TCoord = tcoord;
	worldPos = MCWCMatrix * vertexMC;
	gl_Position = MCDCMatrix * vertexMC;
}