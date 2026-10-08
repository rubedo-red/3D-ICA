#version 460 core

in vec3 aPos;
in vec3 aCol;

out vec3 vertex_colour;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	vertex_colour = aCol;
}
