#version 330 core

out vec4 FragColor;
in vec3 colorout;
in vec2 TexCoord;

uniform vec4 coloru;

uniform sampler2D texture1;


void main()
{
	FragColor = texture(texture1, TexCoord);	
}