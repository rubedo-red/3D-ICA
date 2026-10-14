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
	void render_mesh(Mesh& mesh);

private:
	GLuint mesh_shader{0};

	void set_default_flags();
	void create_mesh_shader();
};
