#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

uniform float lift;

uniform float gamma;

uniform float gain;

void main(){
	vec3 result = texture(screenTexture, texCoords).rgb;

	result = pow(gain * (result + lift * (1 - result)), vec3(gamma));

	FragColor = vec4(result, 1.0);
}