#include "transform.h"

glm::mat4 Transform::matrix() const {
	glm::mat4 transform{1.f};
	transform = glm::translate(transform, translate);
	transform = glm::rotate(transform, roll, {0.f, 0.f, 1.f});
	transform = glm::rotate(transform, pitch, {1.f, 0.f, 0.f});
	transform = glm::rotate(transform, yaw, {0.f, 1.f, 0.f});
	transform = glm::scale(transform, scale);
	return transform;
}
