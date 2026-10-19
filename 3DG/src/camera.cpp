#include "camera.h"

glm::mat4 Camera::calculate_view_matrix() {
	glm::mat4 view_matrix(1.f);
	view_matrix = glm::rotate(view_matrix, -roll, {0.f, 0.f, 1.f});
	view_matrix = glm::rotate(view_matrix, -pitch, {1.f, 0.f, 0.f});
	view_matrix = glm::rotate(view_matrix, -yaw, {0.f, 1.f, 0.f});
	view_matrix = glm::translate(view_matrix, -1.f*position);
	return view_matrix;
}
