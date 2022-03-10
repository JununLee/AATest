#version 450 core
out vec4 FragColor;
in vec2 TCoord;
in  vec4 worldPos;
uniform float time;
uniform vec4 color;
uniform sampler3D myTex;
uniform float scale;
uniform float bias;
uniform mat4 boxlocal2world;

void main(){

vec4 pos = inverse(boxlocal2world) * worldPos;
pos +=0.5;
//if(pos.x<0||pos.y<0){
//	discard;
//}

//vec4 col = texture(myTex,TCoord);
vec4 col = texture(myTex,vec3(TCoord,time));
col.r = col.r * scale + bias;

col.r =  col.r * smoothstep(0.495,0.505,TCoord.y) + (1 - col.r) * (1 - smoothstep(0.495,0.505,TCoord.y));
FragColor = vec4(col.r,col.r,col.r,1);
}
