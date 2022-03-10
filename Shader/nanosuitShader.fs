#version 330 core

out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 worldPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightDir;

void main(){
	
	vec3 ambient = lightColor * 0.1;
	//vec3 lightDir = normalize(lightPos - worldPos);
	vec3 norm = normalize(normal);
	vec3 diffuse = max(dot(norm,lightDir),0) * texture(texture_diffuse1,texCoord).xyz;
	vec3 reflectDir = reflect(-lightDir,norm);
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 specular = pow(max(dot(viewDir,reflectDir),0),32) * texture(texture_specular1,texCoord).xyz * 0.5;
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result,1);
	FragColor = texture(texture_diffuse1,texCoord);
}