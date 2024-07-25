#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

uniform float radius;

uniform float focus;

void main(){
	vec4 result = texture(screenTexture, texCoords);
	float dist = length(texCoords - 0.5) * 2 + (1-radius);
	dist = dist > 0 ? dist : 0;
	result.rgb = result.rgb * (1 - pow(dist, focus));
	FragColor = result;
}