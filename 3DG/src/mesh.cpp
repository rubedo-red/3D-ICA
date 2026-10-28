#include "mesh.h"
#include "stb/stb_image.h"

Mesh::Mesh(const std::vector<VertexData>& verts, const std::vector<GLuint>& indices, const std::vector<std::pair<GLsizei, GLuint>>& areas) {

	create_buffers(verts.data(), verts.size(), indices.data(), indices.size());
	texture_areas = areas;

	std::unordered_set<GLuint> unique_textures;
	for (auto& [count, texture]: texture_areas)
		if (!unique_textures.contains(texture)) {
			textures.push_back(texture);
			unique_textures.emplace(texture);
	}
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(static_cast<GLsizei>(textures.size()), textures.data());
}

void Mesh::create_buffers(
	const VertexData *verts, const size_t num_verts, 
	const GLuint *indices, const size_t num_indices
) {
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, num_verts*sizeof(VertexData), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices*sizeof(GLuint), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(GLfloat)*6));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
