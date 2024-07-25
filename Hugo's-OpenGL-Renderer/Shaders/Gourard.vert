#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

#define MAX_POINT_LIGHTS 4

out VS_OUT {
    vec3 diffuseColor;
	vec3 specularColor;
	vec2 TexCoords;
	vec3 Normal;
} vs_out;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D emission;
};

layout (std140) uniform Matrices {
	mat4 projection;
	mat4 view;
};

layout (std140) uniform CameraScreen {
	vec4 CameraPos;
	vec4 CameraDir;
	vec4 ScreenProps;
};

struct DirLight {	
	vec3 color;
	vec3 direction;
	float specularStrength;
};

struct PointLight {
	vec3 color;
	vec3 position;
	float radius;
};

layout (std140) uniform Lights {
	DirLight mainLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	int pointLightCount;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform mat4 model;
uniform Material material;

vec3 calculateDiffuse(vec3 diffuseLight, vec3 norm, vec3 lightDir){
	float diff = max(dot(norm, lightDir), 0.0);
	return diffuseLight * diff;
}

vec3 calculateSpecular(vec3 specularLight, vec3 norm, vec3 lightDir, vec3 FragPos){
	vec3 viewDir = normalize(CameraPos.xyz - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	return specularLight * spec;
}

vec3 calculateEmission(){
	return texture(material.emission, aTexCoords).rgb;
}

void calcDirLight(vec3 norm, vec3 FragPos){
	vec3 lightDir = normalize(-mainLight.direction);

	vec3 ambient = mainLight.color * 0.2;

	vec3 diffuse = calculateDiffuse(mainLight.color, norm, lightDir);
	vec3 specularColor = mix(vec3(0.0, 0.0, 0.0), mainLight.color, mainLight.specularStrength);
	vec3 specular = calculateSpecular(specularColor, norm, lightDir, FragPos);
	vs_out.specularColor += specular;
	vs_out.diffuseColor += (ambient + diffuse);
}

void calcPointLight(PointLight pointLight, vec3 norm, vec3 FragPos){
	vec3 lightDir = normalize(pointLight.position - FragPos);

	vec3 ambient = pointLight.color * 0.2;

	vec3 diffuse = calculateDiffuse(pointLight.color, norm, lightDir);
	vec3 specularColor = mix(vec3(0.0, 0.0, 0.0), pointLight.color, 0.5);
	vec3 specular = calculateSpecular(specularColor, norm, lightDir, FragPos);

	float lightDist = length(pointLight.position - FragPos); 
	float lightAtt = 1.0/(pow((lightDist/pointLight.radius) + 1, 2));

	ambient *= lightAtt;
	diffuse *= lightAtt;
	specular *= lightAtt;
	vs_out.specularColor += specular;
	vs_out.diffuseColor += (ambient + diffuse);
}

void calcSpotLight(SpotLight spotLight, vec3 norm, vec3 FragPos) {
	vec3 lightDir = normalize(spotLight.position - FragPos);
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
	float intensity = clamp((theta - spotLight.outerCutoff)/epsilon, 0.0, 1.0);

	vec3 ambient = spotLight.ambient * 0.2;

	vec3 diffuse = calculateDiffuse(spotLight.diffuse, norm, lightDir);

	vec3 specular = calculateSpecular(spotLight.specular, norm, lightDir, FragPos);
	float lightDist = length(spotLight.position - FragPos); 
	float lightAtt = 1.0/(spotLight.constant + spotLight.linear*lightDist + spotLight.quadratic * pow(lightDist, 2));

	ambient *= lightAtt;
	diffuse *= lightAtt;
	specular *= lightAtt;
	diffuse  *= intensity;
	specular *= intensity;
	vs_out.specularColor += specular;
	vs_out.diffuseColor += (ambient + diffuse);
}

void main(){
	vs_out.diffuseColor = vec3(0.0);
	vs_out.specularColor = vec3(0.0);
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
	vec3 norm = normalize(Normal);

	calcDirLight(norm, FragPos);

	for(int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++){
		calcPointLight(pointLights[i], norm, FragPos);
	}

	//result += calcSpotLight(spotLight, norm, FragPos);

	//vec3 emission = calculateEmission();

	//vs_out.Color = result;
}