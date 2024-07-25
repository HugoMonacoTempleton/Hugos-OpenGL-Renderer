#version 330
out vec4 FragColor;

in vec2 texCoords;

layout (std140) uniform Time
{
    float time;
    float deltaTime;
};

uniform sampler2D screenTexture;

uniform float intensity;

float hash(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * 1273.0976);
    p3 += dot(p3, p3.yzx + 33.12346);
    return fract((p3.x + p3.y) * p3.z);
}

void main(){
    vec4 col = texture(screenTexture, texCoords);
    float a = 0.0;
    float offset = mod(time, 50.0);
    vec2 pos = vec2(texCoords.x + offset, texCoords.y + offset);
    a += hash(pos);
    col += col * a * intensity;

    FragColor = vec4(vec3(col), 1.0);
}