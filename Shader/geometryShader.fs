#version 330 core

out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D texure_diffuse1;


void main(){
	FragColor = texture(texure_diffuse1,texcoord);
}