#include "skybox.h"
#include <cstdint>
#include <iostream>
#include "stb/stb_image.h"

Skybox::Skybox(
	const std::string& right, 
	const std::string& left, 
	const std::string& top, 
	const std::string& bottom, 
	const std::string& back,
	const std::string& front
) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	// this specific order is needed so that our faces are in the correct places, using pointers to avoid copies 
	const std::string *faces[6] = {&right, &left, &top, &bottom, &front, &back};

	// idk why, but they dont need to be flipped for this
	stbi_set_flip_vertically_on_load(0);
	int width, height, channels;
	for (int i{0}; i<6; i++) {
		uint8_t *data = stbi_load((*faces[i]).c_str(), &width, &height, &channels, 0);
		if (data != nullptr) {
			// combine face textures into one gl texture
			// incrementing enum changes the face we are assigning our texture to
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		} else {
			std::cout << "failed to load skybox texture " << faces[i] << std::endl;
			stbi_image_free(data);
			return;
		}
	}
	stbi_set_flip_vertically_on_load(0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Skybox::~Skybox() {
	glDeleteTextures(1, &texture);
}
