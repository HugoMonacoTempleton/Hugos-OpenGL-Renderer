#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D blendLayer;

void main(){
	vec4 result = texture(screenTexture, texCoords) * texture(blendLayer, texCoords);

	FragColor = result;
}