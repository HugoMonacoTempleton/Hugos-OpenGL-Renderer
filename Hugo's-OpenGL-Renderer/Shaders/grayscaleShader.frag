#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;

void main(){
	vec4 color = texture(screenTexture, texCoords);
	float result = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
	FragColor = vec4(vec3(result), 1.0);
}