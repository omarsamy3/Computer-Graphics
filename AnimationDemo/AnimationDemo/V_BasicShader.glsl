#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec2 vertex_uv;


out vec2 frag_uv;
void main()
{
	gl_Position = vertex_position;
	frag_uv = vertex_uv;
}