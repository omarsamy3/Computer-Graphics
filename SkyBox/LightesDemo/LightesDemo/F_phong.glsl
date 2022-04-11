#version 330

vec4 object_color = vec4(0,0.5,0.5,1);

float ka = 0.5;
float kd = 0.8;
float ks = 20;
float sh = 10;

vec4 Slight_color = vec4(1,1,1,1);
vec4 Dlight_color = vec4(1,1,1,1);
vec4 Alight_color = vec4(1,1,1,1);

in vec3 v;
in vec3 n;
in vec3 l;
in vec3 h;

void main()
{
	vec4 I = ka * Alight_color +
	kd * max(dot(l,n),0) * Dlight_color +
	ks * max(pow(dot(h,n),sh),0) * Slight_color;

	gl_FragColor = object_color * I;
}