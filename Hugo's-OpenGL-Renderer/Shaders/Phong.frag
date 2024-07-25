#version 330 core
layout (location = 0) out vec4 textureColorBuffer;
layout (location = 1) out vec4 depthNormalBuffer;

#define MAX_POINT_LIGHTS 4

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D emission;
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
	
	vec3 color;
	float length;
};

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform Material material;
//uniform DirLight dirLight;
//uniform SpotLight spotLight;


//uniform int NR_POINT_LIGHTS;
//uniform PointLight pointLights[MAX_POINT_LIGHTS];

vec3 calculateDiffuse(vec3 diffuseLight, vec3 norm, vec3 lightDir){
	float diff = max(dot(norm, lightDir), 0.0);
	return diffuseLight * diff * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
}

vec3 calculateSpecular(vec3 specularLight, vec3 norm, vec3 lightDir){
	vec3 viewDir = normalize(CameraPos.xyz - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	return specularLight * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;
}

vec3 calculateEmission(){
	return texture(material.emission, fs_in.TexCoords).rgb;
}

vec3 calcDirLight(vec3 norm){
	vec3 lightDir = normalize(-mainLight.direction);

	vec3 ambient = mainLight.color * texture(material.texture_diffuse1, fs_in.TexCoords).rgb * 0.2;

	vec3 diffuse = calculateDiffuse(mainLight.color, norm, lightDir);
	vec3 specularColor = mix(vec3(0.0, 0.0, 0.0), mainLight.color, mainLight.specularStrength);
	vec3 specular = calculateSpecular(mainLight.color, norm, lightDir);

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight pointLight, vec3 norm){
	vec3 lightDir = normalize(pointLight.position - fs_in.FragPos);

	vec3 ambient = pointLight.color * texture(material.texture_diffuse1, fs_in.TexCoords).rgb * 0.2;

	vec3 diffuse = calculateDiffuse(pointLight.color, norm, lightDir);
	vec3 specularColor = mix(vec3(0.0, 0.0, 0.0), pointLight.color, 0.5);
	vec3 specular = calculateSpecular(pointLight.color, norm, lightDir);

	float lightDist = length(pointLight.position - fs_in.FragPos); 
	float lightAtt = 1.0/(pow((lightDist/pointLight.radius) + 1, 2));

	ambient *= lightAtt;
	diffuse *= lightAtt;
	specular *= lightAtt;
	return (ambient + diffuse + specular);
}

/*vec3 calcSpotLight(SpotLight spotLight, vec3 norm) {
	vec3 lightDir = normalize(spotLight.position - fs_in.FragPos);
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
	float intensity = clamp((theta - spotLight.outerCutoff)/epsilon, 0.0, 1.0);

	vec3 ambient = spotLight.color * texture(material.texture_diffuse1, fs_in.TexCoords).rgb * 0.2;

	vec3 diffuse = calculateDiffuse(spotLight.color, norm, lightDir);

	vec3 specular = calculateSpecular(spotLight.color, norm, lightDir);
	float lightDist = length(spotLight.position - fs_in.FragPos); 
	float lightAtt = 1.0/(pow((lightDist/spotLight.length) + 1, 2));

	ambient *= lightAtt;
	diffuse *= lightAtt;
	specular *= lightAtt;
	diffuse  *= intensity;
	specular *= intensity;
	return ambient + diffuse + specular;
	
}*/

void main(){
	vec3 norm = normalize(fs_in.Normal);

	vec3 result = calcDirLight(norm);

	for(int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++){
		result += calcPointLight(pointLights[i], norm);
	}

	//result += calcSpotLight(spotLight, norm);

	vec3 emission = calculateEmission();

	textureColorBuffer = vec4(result, 1.0);
	depthNormalBuffer.rgb = norm;
	depthNormalBuffer.a = gl_FragCoord.z;


}