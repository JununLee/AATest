#version 450 core
in vec4 vertexMC;
in vec2 tcoordMC;
out vec2 TCoord;
void main(){
TCoord = tcoordMC;
gl_Position = vertexMC;
}