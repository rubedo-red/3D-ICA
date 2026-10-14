#pragma once
#include "Keith_Helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <string>

class Mesh {
public:
	Mesh(std::string model_name, std::string default_texture_override="");
	~Mesh();

	// public since they need to be accessed by the renderer, shouldn't be modified directly unless you really know what you're doing
	GLuint VAO{0};
	glm::mat4 transform{glm::mat4(1.f)};

	std::vector<std::pair<size_t, GLuint>> submesh_textures;

private:
	GLuint VBO{0};
	GLuint EBO{0};
	std::vector<GLuint> textures;

	inline static GLuint default_texture{0};
	void create_default_texture();

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
		std::string texture{""};
	};

	void visit_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& submeshes, std::vector<VertexData>& vertices, std::vector<GLuint>& indices, aiMatrix4x4& p_trans);

	void create_buffers(
		const VertexData *verts, const size_t num_verts, 
		const GLuint *indices, const size_t num_indices
	);

	void create_gltexture(const aiScene *scene, std::string texture_name, GLuint texture_handle);
};
