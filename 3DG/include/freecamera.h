#pragma once
#include "Keith_Helpers.h"
#include "camera.h"

class FreeCamera {
public:
	FreeCamera() = default;
	Camera camera;

	float speed{5.f};
	float rot_speed{2.f};

	bool dirty_movement_vectors{false};
	void move_forward(bool reverse, float dt);
	void move_right(bool reverse, float dt);
	void move_up(bool reverse, float dt);
	void rotate_yaw(bool reverse, float dt);
	void rotate_pitch(bool reverse, float dt);
	void recalculate_movement_vectors();

private:
	glm::vec3 forward_vec{glm::vec3(0.f, 0.f, 1.f)};
	glm::vec3 up_vec{glm::vec3(0.f, 1.f, 0.f)};
	glm::vec3 right_vec{glm::vec3(1.f, 0.f, 0.f)};
};
