#pragma once
#include <vector>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include "Keith_Helpers.h"

class Skybox {
public:
	Skybox(const std::string& right, const std::string& left, const std::string& top, const std::string& bottom, const std::string& back, const std::string& front);
	~Skybox();

	GLuint texture{0};
private:
};
