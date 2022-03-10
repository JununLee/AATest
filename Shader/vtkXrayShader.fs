#version 450 core
out vec4 FragColor;
in vec2 TCoord;
in  vec4 worldPos;
uniform float time;
uniform vec4 color;
uniform sampler3D myTex;
uniform float scale;
uniform float bias;
uniform float minDataValue;
uniform float maxDataValue;
uniform mat4 boxlocal2world;
uniform vec3 cameraPos = vec3(0,0,3);
uniform float threshold = 150;


bool cubeintersection(vec3 origin,vec3 direction,vec3 aabbMax,vec3 aabbMin,out float tNear, out float tFar){
	vec3 invDir = 1.0 / direction;
	vec3 t1 = invDir * (aabbMax - origin);
	vec3 t2 = invDir * (aabbMin - origin);
	vec3 tMin = min(t1,t2);
	vec3 tMax = max(t1,t2);
	tNear = max(max(tMin.x,tMin.y),tMin.z);
	tFar = min(min(tMax.x,tMax.y),tMax.z);
	return tNear<=tFar;
}

void main(){
	vec4 col = vec4(0);
	mat4 boxworld2local = inverse(boxlocal2world);
	vec3 boxPos = (boxworld2local * worldPos).xyz;
	vec3 origin = (boxworld2local * vec4(cameraPos,1)).xyz;
	vec3 dir = normalize(boxPos - origin);
	float tNear,tFar;
	bool intersection = cubeintersection(origin,dir,vec3(-0.5,-0.5,-0.5),vec3(0.5,0.5,0.5),tNear,tFar);
	if(!intersection){
		FragColor = vec4(0,0,0,1);
	}
	else{
		float currentRayLength = tNear;
		float maxSamples = 1024;
		float currentSmaple = 0;
		float stepLength = 1.732/maxSamples;
		float thres = (threshold - minDataValue)/(maxDataValue-minDataValue);
		
		while(currentRayLength<tFar){
			vec3 pos = origin + dir * currentRayLength;
			pos +=0.5;
			vec4 pCol = texture(myTex,pos);
			pCol.r = pCol.r * scale + bias;
			if(pCol.r>thres){
				currentSmaple +=1;
				col.r += (pCol.r - thres)/(1-thres)*(maxSamples - currentSmaple)/maxSamples;
			}
			currentRayLength += stepLength;
		}
		
		col.r = col.r * 0.08;
		FragColor = vec4(col.r,0,0,1);
	}
}
