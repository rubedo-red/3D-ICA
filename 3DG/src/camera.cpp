#include "camera.h"

glm::mat4 Camera::calculate_view_matrix() {
	glm::mat4 view_matrix(1.f);
	view_matrix = glm::rotate(view_matrix, -roll, {0.f, 0.f, 1.f});
	view_matrix = glm::rotate(view_matrix, -pitch, {1.f, 0.f, 0.f});
	view_matrix = glm::rotate(view_matrix, -yaw, {0.f, 1.f, 0.f});
	view_matrix = glm::translate(view_matrix, -1.f*position);
	return view_matrix;
}

void Camera::recalculate_movement_vectors() {
	glm::mat4 rotation(1.f);
	rotation = glm::rotate(rotation, -roll, {0.f, 0.f, 1.f});
	rotation = glm::rotate(rotation, -pitch, {1.f, 0.f, 0.f});
	rotation = glm::rotate(rotation, -yaw, {0.f, 1.f, 0.f});
	forward_vec = glm::vec3(glm::vec4(0.f, 0.f, 1.f, 0.f) * rotation);
	up_vec = glm::vec3(glm::vec4(0.f, 1.f, 0.f, 0.f) * rotation);
	right_vec = glm::vec3(glm::vec4(1.f, 0.f, 0.f, 0.f) * rotation);
}
