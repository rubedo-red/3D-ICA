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

	glm::vec3 position() {return glm::vec3(x, y, z);}
	glm::vec3 normal() {return glm::vec3(nx, ny, nz);}
	void set_position(glm::vec3 pos) {nx=pos.x; ny=pos.y; nz=pos.z;}
	void set_normal(glm::vec3 norm) {nx=norm.x; ny=norm.y; nz=norm.z;}
};

struct Material {
	GLfloat specular{1.f};
	glm::vec3 diffuse{1.f, 1.f, 1.f};
	GLuint diffuse_texture{0};
	GLuint specular_texture{0};
	GLuint normal_texture{0};
};

struct SubmeshInfo {
	GLsizei element_count{0};
	GLsizei start_index{0};
	Material material{};
	glm::mat4 transform{glm::mat4(1.f)};
};

class Mesh {
public:
	Mesh(const std::vector<VertexData>& verts, const std::vector<GLuint>& indices, const std::vector<SubmeshInfo>& sms);
	Mesh(const VertexData* verts, const GLsizei vert_count, const GLuint* indices, const GLsizei index_count, const std::vector<SubmeshInfo>& sms);
	~Mesh();

	// public since they need to be accessed by the renderer, shouldn't be modified directly unless you really know what you're doing
	GLuint VAO{0};
	Transform transform;

	std::vector<SubmeshInfo> submeshes;

private:
	GLuint VBO{0};
	GLuint EBO{0};
	std::vector<GLuint> textures;

	void create_buffers(
		const VertexData *verts, const size_t num_verts, 
		const GLuint *indices, const size_t num_indices
	);
};
