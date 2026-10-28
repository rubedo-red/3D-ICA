#pragma once
#include <vector>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "Keith_Helpers.h"

static constexpr GLuint skybox_indices[] = {
	// back
	0, 1, 2,
	3, 2, 1,
	// front
	6, 5, 4,
	5, 6, 7,
	// top
	4, 1, 0,
	5, 1, 4,
	// bottom
	2, 3, 6,
	7, 6, 3,
	// left
	0, 2, 4,
	6, 4, 2,
	// right
	5, 3, 1,
	3, 5, 7
};

class Skybox {
public:
	Skybox(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& back, const std::string& front);
	~Skybox();

	GLuint texture{0};
private:
};
