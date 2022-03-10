#version 330 core

out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D transparent;

void main(){
	FragColor = texture(transparent,texcoord);
}