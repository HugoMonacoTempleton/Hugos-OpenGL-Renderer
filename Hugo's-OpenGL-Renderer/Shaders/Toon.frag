#version 330 core
layout (location = 0) out vec4 textureColorBuffer;
layout (location = 1) out vec4 depthNormalBuffer;

#define MAX_POINT_LIGHTS 4

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D emission;
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

vec3 calcDirLight(vec3 normal){
	vec3 lightDir = normalize(-mainLight.direction);
	int inLight = dot(normal, lightDir) > 0.0 ? 1 : 0;
	vec3 result = inLight * mainLight.color + (1 - inLight) * mainLight.color * 0.2;

	return result;
}

vec3 calcPointLight(PointLight pointLight, vec3 normal){
	vec3 lightDir = normalize(pointLight.position - fs_in.FragPos);
	
	int inLight = dot(normal, lightDir) > 0.0 ? 1 : 0;

	float lightDist = length(pointLight.position - fs_in.FragPos); 
	float lightAtt = lightDist < pointLight.radius ? 1.0/(pow((lightDist/pointLight.radius) + 1, 2)) : 0.0;

	float QuantAtt = floor((4 - 1.0) * lightAtt + 0.5) / (4 - 1.0);

	vec3 result = QuantAtt * (inLight * pointLight.color + (1 - inLight) * pointLight.color * 0.2);
	return result;
}

void main(){
	vec3 norm = normalize(fs_in.Normal);
	vec3 lightColor = calcDirLight(norm);

	for(int i = 0; i < pointLightCount && i < MAX_POINT_LIGHTS; i++){
		lightColor += calcPointLight(pointLights[i], norm);
	}
	vec3 result = lightColor * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;

	textureColorBuffer = vec4(result, 1.0);
	depthNormalBuffer.rgb = norm;
	depthNormalBuffer.a = gl_FragCoord.z;
}