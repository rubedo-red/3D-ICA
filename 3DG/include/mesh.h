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
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat nx;
		GLfloat ny;
		GLfloat nz;
		GLfloat u;
		GLfloat v;
	};

	struct Submesh {
		std::vector<VertexData> vertices;
		std::vector<GLuint> indices;
		const aiTexture *texture{nullptr};
	};

	void visit_node(const aiScene *scene, aiNode *sm, std::vector<Submesh>& list, aiMatrix4x4& p_trans, GLuint base_vert=0);

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
