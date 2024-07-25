#version 330
out vec4 FragColor;

in vec2 texCoords;

layout (std140) uniform CameraScreen {
	vec4 CameraPos;
	vec4 CameraDir;
	vec4 ScreenProps;
};

float near = 0.1;
float far = 100.0;

uniform sampler2D screenTexture;

uniform sampler2D normalDepthTexture;

uniform float _DepthThreshold;

uniform float _NormalThreshold;

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

	float sampleDepth[9];
	for(int i = 0; i < 9; i++){
		sampleDepth[i] = texture(normalDepthTexture, texCoords + offsets[i]).a;
	}

	float depthDiffx = 0.0;
	for(int i = 0; i < 9; i++){
		depthDiffx += sampleDepth[i] * Gx[i];
	}

	float depthDiffy;
	for(int i = 0; i < 9; i++){
		depthDiffy += sampleDepth[i] * Gy[i];
	}

	float edgeDepth = sqrt(pow(depthDiffx, 2) + pow(depthDiffy, 2));

	float depthThreshold = _DepthThreshold * sampleDepth[4];

	edgeDepth = edgeDepth > depthThreshold ? 1.0 : 0.0;

	vec3 sampleNormal[9];
	for(int i = 0; i < 9; i++){
		sampleNormal[i] = texture(normalDepthTexture, texCoords + offsets[i]).rgb;
	}

	vec3 normalDiffx = vec3(0.0);
	for(int i = 0; i < 9; i++){
		normalDiffx += sampleNormal[i] * Gx[i];
	}

	vec3 normalDiffy = vec3(0.0);
	for(int i = 0; i < 9; i++){
		normalDiffy += sampleNormal[i] * Gy[i];
	}

	float edgeNormal = sqrt(dot(normalDiffx, normalDiffx) + dot(normalDiffy, normalDiffy));
	edgeNormal = edgeNormal > _NormalThreshold ? 1.0 : 0.0;

	FragColor = vec4(vec3(1 - max(edgeDepth, edgeNormal)), 1.0);
	
}