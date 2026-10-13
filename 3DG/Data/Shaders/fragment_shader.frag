#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;

uniform sampler2D text;

void main(void)
{
	fragment_colour = texture(text, uv);
}
