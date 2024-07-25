#version 330 core
out vec4 FragColor;
  
in vec2 texCoords;

layout (std140) uniform CameraScreen {
	vec4 CameraPos;
	vec4 CameraDir;
	vec4 ScreenProps;
};

uniform sampler2D screenTexture;

void main()
{ 
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

	float kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++){
		sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++){
		col += sampleTex[i] * kernel[i];
	}
    FragColor = vec4(col, 1.0);
}