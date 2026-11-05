#version 460 core

layout(location = 0) out vec4 fragment_colour;
in vec3 norm;
in vec2 uv;

uniform sampler2D text;
uniform vec4 color = vec4(1.f, 1.f, 1.f, 1.f);
uniform vec3 dir_light = {0.4f, -0.3f, 0.0f};
uniform float global_light = 0.4f;
uniform bool enable_lighting = true;

void main(void)
{
	float lightness = 1.f;
	if (enable_lighting)
		lightness = clamp(dot(normalize(norm), -normalize(dir_light)), 0.f, 1.f - global_light) + global_light;
		
	fragment_colour = color * texture(text, uv) * lightness;
}
