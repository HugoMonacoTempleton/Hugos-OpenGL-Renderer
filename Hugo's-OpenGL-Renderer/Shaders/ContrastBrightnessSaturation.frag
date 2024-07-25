#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

uniform float contrast;

uniform float brightness;

uniform float saturation;

void main(){
	vec4 result = texture(screenTexture, texCoords);

	result.rgb = contrast * (result.rgb - 0.5) + 0.5 + brightness;

	float greyValue = 0.299 * result.r + 0.587 * result.g + 0.114 * result.b;
	
	result.rgb = saturation * result.rgb + (1-saturation) * greyValue;

	FragColor = result;
}