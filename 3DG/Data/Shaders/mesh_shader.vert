#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;

out vec3 norm;
out vec2 uv;
out vec3 world_pos;

uniform mat4 obj_transform = mat4(1.f);
uniform mat4 view_transform = mat4(1.f);

void main()
{
	world_pos = vec3(obj_transform * vec4(aPos, 1.f));
	gl_Position = view_transform * obj_transform * vec4(aPos, 1.f);
	norm = normalize(vec3(obj_transform * vec4(aNorm, 0.f)));
	uv = aUV;
}
