#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;

uniform sampler2D text;
uniform vec4 color;

void main(void)
{
	fragment_colour = color * texture(text, uv);
}
