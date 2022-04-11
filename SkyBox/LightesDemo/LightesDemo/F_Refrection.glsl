#version 330

in vec3 frag_position;
in vec3 frag_normal;

uniform samplerCube cubemap;  
uniform vec3 camera_position;

void main()
{             
	vec3 r = refract(normalize(frag_position-camera_position),normalize(frag_normal),1/1.52);
    gl_FragColor = texture(cubemap, r);
}