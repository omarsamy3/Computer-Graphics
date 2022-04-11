#version 330

uniform samplerCube cubemap;  

in vec3 frag_uv;
void main()
{             
    gl_FragColor = texture(cubemap, frag_uv);
}