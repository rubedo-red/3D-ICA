#include "freecamera.h"

void FreeCamera::recalculate_movement_vectors() {
	glm::mat4 rotation(1.f);
	rotation = glm::rotate(rotation, -camera.roll, {0.f, 0.f, 1.f});
	rotation = glm::rotate(rotation, -camera.pitch, {1.f, 0.f, 0.f});
	rotation = glm::rotate(rotation, -camera.yaw, {0.f, 1.f, 0.f});
	forward_vec = glm::vec3(glm::vec4(0.f, 0.f, 1.f, 0.f) * rotation);
	up_vec = glm::vec3(glm::vec4(0.f, 1.f, 0.f, 0.f) * rotation);
	right_vec = glm::vec3(glm::vec4(1.f, 0.f, 0.f, 0.f) * rotation);
}

void FreeCamera::move_forward(bool reverse, float dt) {
	camera.position += forward_vec * dt * speed * ((reverse) ? 1.f : -1.f);
}

void FreeCamera::move_right(bool reverse, float dt) {
	camera.position += right_vec * dt * speed * ((reverse) ? -1.f : 1.f);
}

void FreeCamera::move_up(bool reverse, float dt) {
	camera.position += up_vec * dt * speed * ((reverse) ? -1.f : 1.f);
}

void FreeCamera::rotate_yaw(bool reverse, float dt) {
	camera.yaw =  std::fmod(camera.yaw + rot_speed * dt * ((reverse) ? -1.f : 1.f), 6.2831853f);
	dirty_movement_vectors = true;
}

void FreeCamera::rotate_pitch(bool reverse, float dt) {
	camera.pitch =  std::clamp(camera.pitch + rot_speed * dt * ((reverse) ? -1.f : 1.f), -1.570796f, 1.570795f);
	dirty_movement_vectors = true;
}
