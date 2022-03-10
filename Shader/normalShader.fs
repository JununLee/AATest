#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 worldPos;
in mat3 TBN;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightDir;

void main(){
	
	vec3 ambient = lightColor * 0.1;
	//vec3 lightDir = normalize(lightPos - worldPos);
	vec3 norm = texture(texture2,texCoord).xyz;
	norm = normalize(norm * 2 - 1);
	norm.xy *=1;
	norm.z = sqrt(1 - (norm.x*norm.x+norm.y*norm.y));
	norm = normalize(TBN * norm);
	vec3 diffuse = max(dot(norm,lightDir),0) * texture(texture1,texCoord).xyz;
	vec3 reflectDir = reflect(-lightDir,norm);
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 result = ambient + diffuse;
	FragColor = vec4(result,1);
}