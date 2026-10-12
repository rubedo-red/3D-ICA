#pragma once
#include "Keith_Helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>


class Mesh {
public:
	Mesh(const aiScene *scene);
	~Mesh();

	void render(glm::vec3 project);

private:
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

	struct Submesh {
		size_t verts_begin{0};
		size_t verts_count{0};
		size_t indices_begin{0};
		size_t indices_count{0};
		const aiTexture *texture{nullptr};
	};

	void visit_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& submeshes, std::vector<VertexData>& vertices, std::vector<GLuint>& indices, aiMatrix4x4& p_trans);

	void create_buffers(
		const VertexData *verts, const size_t num_verts, 
		const GLuint *indices, const size_t num_indices
	);

	GLuint create_gltexture(const aiTexture* texture);

	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	glm::mat4 transform;
	std::vector<std::pair<size_t, GLuint>> textures;
};
