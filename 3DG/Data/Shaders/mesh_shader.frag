#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;
in vec3 world_pos;

uniform sampler2D obj_texture;
uniform vec3 obj_color = vec3(1.f, 1.f, 1.f);
uniform vec3 light_dir = {0.4f, -0.6f, -0.3f};
uniform vec3 light_ambient = {0.4f, 0.4f, 0.4f};
uniform vec3 view_pos = {0.f, 0.f, 0.f};
uniform bool light_enable = true;

void main(void)
{
	if (light_enable) {
		float diffuse_strength = max(dot(normalize(norm), -normalize(light_dir)), 0);
		vec3 diffuse = vec3(diffuse_strength);
		vec3 result = (diffuse + light_ambient) * obj_color * vec3(texture(obj_texture, uv));
		fragment_colour = vec4(result, 1.f);
	} 
	else fragment_colour = (vec4(obj_color, 1.f) * texture(obj_texture, uv));
}
