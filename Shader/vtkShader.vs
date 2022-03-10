#version 450 core
in vec4 vertexMC;
in vec2 tcoord;
out vec2 TCoord;
out vec4 worldPos;
uniform mat4 MCDCMatrix;
uniform mat3 rot;
uniform mat4 MCWCMatrix;
void main(){
TCoord = tcoord;
worldPos = MCWCMatrix * vertexMC;
//gl_Position = MCDCMatrix * vec4(rot * vertexMC.xyz,1);
gl_Position = MCDCMatrix * vertexMC;
}