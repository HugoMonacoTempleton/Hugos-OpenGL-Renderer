#version 330
out vec4 FragColor;

in vec2 texCoords;

layout (std140) uniform CameraScreen {
	vec4 CameraPos;
	vec4 CameraDir;
	vec4 ScreenProps;
};

uniform sampler2D screenTexture;

uniform float radius;

uniform float focus;

uniform float redOffsetAngle;
uniform float redOffsetStrength;

uniform float greenOffsetAngle;
uniform float greenOffsetStrength;

uniform float blueOffsetAngle;
uniform float blueOffsetStrength;

void main() {
	vec4 result = vec4(1.0);
	float strength = length(texCoords - 0.5) * 2 + (1-radius);
	strength = strength > 0 ? strength : 0;
	strength = pow(strength, focus);
	strength = strength <= 1 ? strength : 1;
	vec2 redOffset = strength * redOffsetStrength * vec2(ScreenProps.x * cos(redOffsetAngle), ScreenProps.y * sin(redOffsetAngle));
	vec2 greenOffset = strength * greenOffsetStrength * vec2(ScreenProps.x * cos(greenOffsetAngle), ScreenProps.y * sin(greenOffsetAngle));
	vec2 blueOffset = strength * blueOffsetStrength *vec2(ScreenProps.x * cos(blueOffsetAngle), ScreenProps.y * sin(blueOffsetAngle));
	result.r = texture(screenTexture, texCoords + redOffset).r;
	result.g = texture(screenTexture, texCoords + greenOffset).g;
	result.b = texture(screenTexture, texCoords + blueOffset).b;
	FragColor = result;
}