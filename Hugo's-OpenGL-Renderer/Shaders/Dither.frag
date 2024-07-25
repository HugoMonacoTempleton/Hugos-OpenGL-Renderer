#version 330
out vec4 FragColor;

in vec2 texCoords;

uniform vec4 texelSize;

uniform sampler2D screenTexture;

uniform float spread;

uniform int redColorCount;

uniform int greenColorCount;

uniform int blueColorCount;

uniform int bayerLevel;

const int bayer2[2 * 2] = int[](
    0, 2,
    3, 1
);

const int bayer4[4 * 4] = int[](
    0, 8, 2, 10,
    12, 4, 14, 6,
    3, 11, 1, 9,
    15, 7, 13, 5
);

const int bayer8[8 * 8] = int[](
    0, 32, 8, 40, 2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,  
    12, 44,  4, 36, 14, 46,  6, 38, 
    60, 28, 52, 20, 62, 30, 54, 22,  
    3, 35, 11, 43,  1, 33,  9, 41,  
    51, 19, 59, 27, 49, 17, 57, 25, 
    15, 47,  7, 39, 13, 45,  5, 37, 
    63, 31, 55, 23, 61, 29, 53, 21
);

float GetBayer2(int x, int y) {
    int index = int(mod(x, 2) + mod(y, 2) * 2);
    return float(bayer2[index]) * (1.0 / 4.0) - 0.5;
}

float GetBayer4(int x, int y) {
    int index = int(mod(x, 4) + mod(y, 4) * 4);
    return float(bayer4[index]) * (1.0 / 16.0) - 0.5;
}

float GetBayer8(int x, int y) {
    int index = int(mod(x, 8) + mod(y, 8) * 8);
    return float(bayer8[index]) * (1.0 / 64.0) - 0.5;
}


void main(){
    vec4 result = texture(screenTexture, texCoords);

    int x = int(texCoords.x * texelSize.z);
    int y = int(texCoords.y * texelSize.w);

    float bayerValues[3] = float[]( 0.0, 0.0, 0.0 );
    bayerValues[0] = GetBayer2(x, y);
    bayerValues[1] = GetBayer4(x, y);
    bayerValues[2] = GetBayer8(x, y);

    result = result + spread * bayerValues[bayerLevel];

    result.r = floor((redColorCount - 1.0) * result.r + 0.5) / (redColorCount - 1.0);
    result.g = floor((greenColorCount - 1.0) * result.g + 0.5) / (greenColorCount - 1.0);
    result.b = floor((blueColorCount - 1.0) * result.b + 0.5) / (blueColorCount - 1.0);

	FragColor = result;
    //FragColor = vec4(texCoords.x, texCoords.y, 0.0, 1.0);
}