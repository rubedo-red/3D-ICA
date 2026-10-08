#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 vertex_colour;

void main(void)
{
	fragment_colour = vec4(vertex_colour, 1.0);
}
