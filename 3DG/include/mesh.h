#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <string>
#include "Keith_Helpers.h"
#include "transform.h"

struct VertexData {
	GLfloat x{0.f};
	GLfloat y{0.f};
	GLfloat z{0.f};
	GLfloat nx{0.f};
	GLfloat ny{0.f};
	GLfloat nz{0.f};
	GLfloat u{0.f};
	GLfloat v{0.f};
};

class Mesh {
public:
	Mesh(const std::vector<VertexData>& verts, const std::vector<GLuint>& indices, const std::vector<std::pair<GLsizei, GLuint>>& areas);
	Mesh(const VertexData* verts, const GLsizei vert_count, const GLuint* indices, const GLsizei index_count, const std::vector<std::pair<GLsizei, GLuint>>& areas);
	~Mesh();

	// public since they need to be accessed by the renderer, shouldn't be modified directly unless you really know what you're doing
	GLuint VAO{0};
	Transform transform;

	std::vector<std::pair<GLsizei, GLuint>> texture_areas;

private:
	GLuint VBO{0};
	GLuint EBO{0};
	std::vector<GLuint> textures;

	void create_buffers(
		const VertexData *verts, const size_t num_verts, 
		const GLuint *indices, const size_t num_indices
	);
};
