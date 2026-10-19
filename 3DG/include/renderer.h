#pragma once
#include "Keith_Helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"

class Renderer {
public:
	Renderer();
	~Renderer();

	void clear();
	void render_mesh(const Mesh& mesh, const glm::mat4& view_matrix);
	void set_projection(float fov_y, float aspect, float near_dist, float far_dist);

	glm::mat4 projection_matrix{glm::mat4{1.f}};

private:
	GLuint mesh_shader{0};

	void set_default_flags();
	void create_mesh_shader();
};
