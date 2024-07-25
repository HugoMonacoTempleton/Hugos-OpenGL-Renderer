#version 330 core
out vec4 FragColour;

in vec3 texCoords;

uniform samplerCube skyBox;

void main(){
	FragColour = texture(skyBox, texCoords);
}