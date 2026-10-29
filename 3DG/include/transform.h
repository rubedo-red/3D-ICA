#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform {
	Transform() = default;
	~Transform() = default;

	glm::vec3 translate{glm::vec3(0.f, 0.f, 0.f)};
    glm::vec3 scale{glm::vec3{1.f, 1.f, 1.f}};
	float roll{0.f};
	float pitch{0.f};
	float yaw{0.f};

	glm::mat4 matrix() const;
};
