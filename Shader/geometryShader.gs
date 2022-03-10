#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 texcoord;
}gs_in[];

out vec2 texcoord;

uniform float time;

vec3 GetNormal(){
	vec3 a = normalize(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz);
	vec3 b = normalize(gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);
	return  normalize(cross(a,b));
}

vec4 explode(vec4 position,vec3 normal){
	float magnit1ude = 2;
	vec3 direction = normal * ((sin(time) + 1)/2.0) * magnit1ude;
	return position + vec4(direction,0);
}

void main(){
	vec3 normal = GetNormal();
	gl_Position = explode(gl_in[0].gl_Position,normal);
	texcoord = gs_in[0].texcoord;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position,normal);
	texcoord = gs_in[1].texcoord;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position,normal);
	texcoord = gs_in[2].texcoord;
	EmitVertex();
	EndPrimitive();
}