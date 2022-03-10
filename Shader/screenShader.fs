#version 330 core

out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D screenTex;

const float xoffset = 1/400.0;
const float yoffset = 1/300.0;

void main(){
	
	vec2 offsets[9] = vec2[](vec2(-xoffset,yoffset),
							 vec2(0,yoffset),
							 vec2(xoffset,yoffset),
							 vec2(-xoffset,0),
							 vec2(0,0),
							 vec2(xoffset,0),
							 vec2(-xoffset,-yoffset),
							 vec2(0,-yoffset),
							 vec2(xoffset,-yoffset)
	);
	
	float kerner[9] = float[](1.0,1.0,1.0,
							  1.0,-8.0,1.0,
							  1.0,1.0,1.0
	);
	
	vec3 col = vec3(0);
	for(int i = 0; i < 9; i++){
		vec3 sample = vec3(texture(screenTex,texcoord + offsets[i]));
		col += sample * kerner[i];
	}
	FragColor = vec4(texture(screenTex,texcoord).rgb,1);
	//if(gl_FragCoord.x<400)
		//FragColor = vec4(1,0,0,1);
	//FragColor = vec4(texture(screenTex,texcoord).xyz,1);
	//vec4 color = texture(screenTex,texcoord);
	//FragColor = vec4(1 - color.xyz,1);
}