#pragma once
#include "Keith_Helpers.h"

struct Camera {
public:
	Camera() = default;
	glm::vec3 position{glm::vec3(0.f, 0.f, 0.f)};
	float roll{0.f};
	float pitch{0.f};
	float yaw{0.f};

	glm::vec3 forward_vec{glm::vec3(0.f, 0.f, 1.f)};
	glm::vec3 up_vec{glm::vec3(0.f, 1.f, 0.f)};
	glm::vec3 right_vec{glm::vec3(1.f, 0.f, 0.f)};

	glm::mat4 calculate_view_matrix();
	void recalculate_movement_vectors();
private:
};
