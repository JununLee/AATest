#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 worldPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D shadowMap;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
//uniform vec3 lightDir;

float ShadowCalculation(vec4 fragPosLightSpace){
	vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	vec3 nor = normalize(normal);
	vec3 lightDir = normalize(lightPos - worldPos);
	float bias = max(0.05 * (1 - dot(nor,lightDir)),0.005)*0.5;
	float closest = texture(shadowMap,projCoords.xy).x + bias;
	float currentdepth = projCoords.z;
	
	float shadow = 0.0;
	vec2 texSize = 1.0/textureSize(shadowMap,0);
	for(int x = -2;x <= 2; ++x){
		for(int y = -2;y <= 2; ++y){
			float pcfdepth = texture(shadowMap,projCoords.xy + vec2(x,y) * texSize).x + bias;
			shadow +=currentdepth>pcfdepth?1.0:0.0;
		}
	}
	shadow /= 25.0;
	//shadow =currentdepth>closest?1.0:0.0;
	if(projCoords.z>1)
		shadow = 0;
	
	return shadow;
}

void main(){
	
	vec3 ambient = lightColor * 0.1;
	vec3 lightDir = normalize(lightPos - worldPos);
	vec3 norm = normalize(normal);
	vec3 diffuse = max(dot(norm,lightDir)/2 + 0.5,0) * texture(texture1,texCoord).xyz;
	vec3 reflectDir = reflect(-lightDir,norm);
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 specular = pow(max(dot(viewDir,reflectDir),0),32) * texture(texture2,texCoord).xyz * 0.5;
	vec3 result = ambient + diffuse + specular;
	//if(ShadowCalculation(FragPosLightSpace)>0.5){
	//	result *=0.2; 
	//}
	float shadow = min(ShadowCalculation(FragPosLightSpace),0.75);
	result *= (1 - shadow);
	FragColor = vec4(result,1);
	
}