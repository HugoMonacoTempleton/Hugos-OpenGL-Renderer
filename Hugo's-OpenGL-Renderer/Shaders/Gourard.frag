#version 330 core
layout (location = 0) out vec4 textureColorBuffer;
layout (location = 1) out vec4 depthNormalBuffer;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D emission;
};

in VS_OUT {
	vec3 diffuseColor;
	vec3 specularColor;
	vec2 TexCoords;
	vec3 Normal;
} fs_in;

uniform Material material;

void main(){
	vec3 norm = normalize(fs_in.Normal);
	vec3 result = fs_in.diffuseColor * texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
	result += fs_in.specularColor * texture(material.texture_specular1, fs_in.TexCoords).rgb;
	textureColorBuffer = vec4(result, 1.0);
	depthNormalBuffer.rgb = norm;
	depthNormalBuffer.a = gl_FragCoord.z;
}