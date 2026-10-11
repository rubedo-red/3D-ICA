#version 460 core

in vec3 aPos;
in vec3 aCol;

out vec3 vertex_colour;

uniform mat4 transform;

void main()
{
	gl_Position = transform * (vec4(aPos, 1.0) - vec4(0, 0, 0.5, 0)) + vec4(0, 0, 0.5, 0);
	vertex_colour = aCol;
}
