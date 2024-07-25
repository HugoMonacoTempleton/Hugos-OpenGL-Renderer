#version 330 core
out vec4 FragColor;

in vec2 texCoords;

layout (std140) uniform CameraScreen {
	vec4 CameraPos;
	vec4 CameraDir;
	vec4 ScreenProps;
};

uniform sampler2D screenTexture;

uniform float _Threshold;

void main(){
	float xOffset = ScreenProps.x;
	float yOffset = ScreenProps.y;
	vec2 offsets[9] = vec2[](
		vec2(-xOffset, yOffset),
		vec2(0.0f, yOffset),
		vec2(xOffset, yOffset),
		vec2(-xOffset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(xOffset, 0.0f),
		vec2(-xOffset, -yOffset),
		vec2(0.0f, -yOffset),
		vec2(xOffset, -yOffset)
	);

	float Gx[9] = float[](
		1, 0, -1,
		2, 0, -2,
		1, 0, -1
	);

	float Gy[9] = float[](
		1,  2,  1,
		0,  0,  0,
	   -1, -2, -1
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++){
		sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));
	}

	vec3 GxVal = vec3(0.0);
	for(int i = 0; i < 9; i++){
		GxVal += sampleTex[i] * Gx[i];
	}

	vec3 GyVal = vec3(0.0);
	for(int i = 0; i < 9; i++){
		GyVal += sampleTex[i] * Gy[i];
	}

	float Gval = 1.0 - step(length(GxVal) + length(GyVal), _Threshold);


	FragColor = vec4(vec3(1.0, 1.0, 1.0) - vec3(Gval), 1.0);
}