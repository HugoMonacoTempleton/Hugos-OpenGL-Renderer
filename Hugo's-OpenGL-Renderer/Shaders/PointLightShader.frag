#version 330
out vec4 FragColor;

in VS_OUT {
	vec2 TexCoords;
} fs_in;

uniform sampler2D _MainTex;
uniform vec3 color;

void main(){
	vec4 result = texture(_MainTex, fs_in.TexCoords);
	result.rgb *= color;
	result.rgb *= result.a; 
	FragColor = result;
}