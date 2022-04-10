#version 330

uniform sampler2D image;

int COLS_COUNT = 5;
int ROWS_COUNT = 2;

uniform int current_x;
uniform int current_y;

in vec2 frag_uv;
void main()
{
	vec2 frameSize = vec2( 1.0 / COLS_COUNT, 1.0 / ROWS_COUNT);

	vec2 uv = vec2 ((frag_uv.x / COLS_COUNT) + current_x * frameSize.x
				,(1 - frag_uv.y) / ROWS_COUNT + current_y * frameSize.y);

	gl_FragColor = texture(image,uv);
}