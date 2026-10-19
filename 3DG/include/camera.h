#pragma once
#include "Keith_Helpers.h"

struct Camera {
public:
	Camera() = default;
	glm::vec3 position{glm::vec3(0.f, 0.f, 0.f)};
	float roll{0.f};
	float pitch{0.f};
	float yaw{0.f};

	glm::mat4 calculate_view_matrix();
private:
};
