#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout (std140) uniform Matrices {
	mat4 projection;
	mat4 view;
};

void main(){
	texCoords = aPos;
	mat4 skyview = mat4(mat3(view));
	vec4 pos = projection * skyview * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}