#version 330

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 vertex_normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

vec3 v;
vec3 n;
vec3 l;
vec3 h;

float ka = 0.5;
float kd = 0.8;
float ks = 20;
float sh = 10;

vec3 camera_position = vec3(0,0,3);
vec3 light_distance = vec3(1,1,1);

vec4 Slight_color = vec4(1,1,1,1);
vec4 Dlight_color = vec4(1,1,1,1);
vec4 Alight_color = vec4(1,1,1,1);

vec4 object_color = vec4(0,0.5,0.5,1);

out vec4 frag_color;
void main()
{
	gl_Position = projMat * viewMat * modelMat * vertex_position;

	l = normalize(light_distance);
	n = normalize((modelMat * normalize(vertex_normal)).xyz);
	v = normalize(camera_position - (modelMat*vertex_position).xyz);
	h = normalize(v + l);

	vec4 I = ka * Alight_color +
	kd * max(dot(l,n),0) * Dlight_color +
	ks * max(pow(dot(h,n),sh),0) * Slight_color;

	frag_color = object_color * I;
}