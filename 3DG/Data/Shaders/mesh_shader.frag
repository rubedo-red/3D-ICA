#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;
in vec3 world_pos;

uniform sampler2D obj_texture;

uniform vec3 obj_color = vec3(1.f, 1.f, 1.f);
uniform vec3 light_dir = {0.4f, -0.6f, -0.3f};
uniform vec3 light_dir_col = {1.f, 1.f, 1.0f};
uniform vec3 light_ambient = {0.3f, 0.30f, 0.3f};
uniform vec3 view_pos = {0.f, 0.f, 0.f};

uniform float specular_value = 0.4f;

uniform bool light_enable = true;

void main(void)
{
	if (light_enable) {
		vec3 L = normalize(light_dir);
		vec3 N = normalize(norm);

		float diffuse_strength = max(dot(N, -L), 0);
		vec3 diffuse = diffuse_strength * light_dir_col;
		
		vec3 view_dir = normalize(view_pos - world_pos);
		vec3 reflect_dir = reflect(-L, N);
		float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
		vec3 specular = vec3(specular_value * spec * light_dir_col);

		vec3 result = (diffuse + light_ambient) * obj_color * vec3(texture(obj_texture, uv)) + specular;
		fragment_colour = vec4(result, 1.f);
	} 
	else fragment_colour = (vec4(obj_color, 1.f) * texture(obj_texture, uv));
}
