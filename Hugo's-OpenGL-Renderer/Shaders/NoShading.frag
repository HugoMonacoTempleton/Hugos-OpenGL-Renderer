#version 330 core
layout (location = 0) out vec4 textureColorBuffer;
layout (location = 1) out vec4 depthNormalBuffer;

in VS_OUT {
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


uniform sampler2D texture1;

void main()
{   
	vec3 norm = normalize(fs_in.Normal);
    textureColorBuffer = texture(texture1, fs_in.TexCoords);
	depthNormalBuffer.rgb = norm;
	depthNormalBuffer.a = gl_FragCoord.z;
}