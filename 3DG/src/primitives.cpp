#include "primitives.h"

std::vector<SubmeshInfo> cube_primitive_submeshes() {
	std::vector<SubmeshInfo> result;
	for (size_t i = 0; i < 6; i++) {
		Material mat = {
			.specular = 0.5f,
			.diffuse = primitive_colors[i%(sizeof(primitive_colors)/sizeof(glm::vec3))]
		};
		SubmeshInfo sm = {
			.element_count = 6,
			.start_index = 6*i,
			.material = mat
		};
		result.push_back(sm);
	}
	return result;
}

void create_default_texture() {
	static constexpr GLubyte default_texture_data[] = {
		255, 255, 255
	};
	// this must live for the entire duration of the program, so we dont need to worry about deletion
	glGenTextures(1, &default_texture);
	glBindTexture(GL_TEXTURE_2D, default_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, default_texture_data);
	glBindTexture(GL_TEXTURE_2D, 0);
}
