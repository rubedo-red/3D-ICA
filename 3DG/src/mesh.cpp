#include "mesh.h"

Mesh::Mesh(const std::vector<VertexData>& verts, const std::vector<GLuint>& indices, const std::vector<SubmeshInfo>& sms) {
	create_buffers(verts.data(), verts.size(), indices.data(), indices.size());
	submeshes = sms;
}

Mesh::Mesh(const VertexData* verts, const GLsizei vert_count, const GLuint* indices, const GLsizei index_count, const std::vector<SubmeshInfo>& sms) {
	create_buffers(verts, vert_count, indices, index_count);
	submeshes = sms;
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
	for (auto& sm: submeshes) {
		glDeleteTextures(3, &sm.material.diffuse_texture);
	}
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
