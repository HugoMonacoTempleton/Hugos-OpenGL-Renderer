#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec4 Colour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 calculateDiffuse(vec3 norm, vec3 lightDir){
	float diff = max(dot(norm, lightDir), 0.0);
	return diff * lightColour;
}

vec3 calculateSpecular(vec3 vertPos, vec3 norm, vec3 lightDir, float specularStrength){
	vec3 viewDir = normalize(viewPos - vertPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	return specularStrength * spec * lightColour;
}

void main(){
	float ambientStrength = 0.1;
	float specularStrength = 0.5;

	vec3 vertPos = vec3(model * vec4(aPos, 1.0));

	vec3 norm = vec3(normalize(model * vec4(aNormal, 1.0)));

	vec3 lightDir = normalize(lightPos - vertPos);

	vec3 ambient = ambientStrength * lightColour;

	vec3 diffuse = calculateDiffuse(norm, lightDir);

	vec3 specular = calculateSpecular(vertPos, norm, lightDir, specularStrength);

	vec3 result = (ambient + diffuse + specular) * objectColour;
	Colour = vec4(result, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);

}

