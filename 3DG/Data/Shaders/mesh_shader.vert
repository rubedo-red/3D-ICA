#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;

out vec3 norm;
out vec2 uv;

uniform mat4 mesh_transform = mat4(1.f);
uniform mat4 view_transform = mat4(1.f);

void main()
{
	gl_Position = view_transform * mesh_transform * vec4(aPos, 1.f);
	norm = normalize(vec3(mesh_transform * vec4(aNorm, 0.f)));
	uv = aUV;
}
