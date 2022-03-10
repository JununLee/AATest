#version 450 core
out vec4 FragColor;
in vec2 TCoord;
uniform sampler2D source;
uniform sampler2D source1;
float xp = 1.0/800;
float yp = 1.0/800;
vec2 uvs[9];

float Sobel(){
	float Gx[9] = {-1,-1,-1,0,0,0,1,1,1};
	float Gy[9] = {-1,0,1,-1,0,1,-1,0,1};
	float texColor;
	float edgeX = 0;
	float edgeY = 0;
	for(int i = 0;i < 9; i++){
		vec4 c = texture(source1,uvs[i]);
		texColor = c.r * 0.299 + c.g * 0.587 + c.b * 0.144;
		edgeX +=texColor * Gx[i];
		edgeY +=texColor * Gy[i];
	}
	float edge = max(1 - abs(edgeX) - abs(edgeY),0);
	return edge;
}
void main(){
	uvs[0] = TCoord + vec2(-1,-1) * vec2(xp,yp);
	uvs[1] = TCoord + vec2(0,-1) * vec2(xp,yp);
	uvs[2] = TCoord + vec2(1,-1) * vec2(xp,yp);
	uvs[3] = TCoord + vec2(-1,0) * vec2(xp,yp);
	uvs[4] = TCoord + vec2(0,0) * vec2(xp,yp);
	uvs[5] = TCoord + vec2(1,0) * vec2(xp,yp);
	uvs[6] = TCoord + vec2(-1,1) * vec2(xp,yp);
	uvs[7] = TCoord + vec2(0,1) * vec2(xp,yp);
	uvs[8] = TCoord + vec2(1,1) * vec2(xp,yp);
	
	float edge = Sobel();
	vec4 color = texture(source,TCoord) * (edge) + vec4(1,1,1,1) * (1 - edge) ;
	//vec4 color = texture(source1,TCoord);
	FragColor = vec4(color.r,color.g,color.b,1);
}
