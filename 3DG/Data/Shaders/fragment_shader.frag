#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;

void main(void)
{
	fragment_colour = vec4(uv, 0, 1);
}
