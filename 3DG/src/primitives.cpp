#include "primitives.h"

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