#version 330

layout(location = 0) in vec3 vertex_position;
layout (location = 1) in vec4 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 frag_uv;
void main()
{
	gl_Position = projMat * viewMat * modelMat * vec4 (vertex_position , 1 );

	frag_uv = vertex_position;
}