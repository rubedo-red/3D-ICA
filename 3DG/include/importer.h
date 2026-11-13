#pragma once


#include "stb/stb_image.h"
#include "mesh.h"

#include <filesystem>

Mesh model_to_mesh(const std::string& model);

// caller is resposible for ownership of returned texture
GLuint texture_from_ai(const aiScene *scene, std::filesystem::path texture_path, std::filesystem::path parent_path);

void visit_ai_node(aiNode *node, std::vector<SubmeshInfo>& unique_meshes, std::vector<SubmeshInfo>& parts, glm::mat4 p_trans=glm::mat4(1.f));

struct Image {
	~Image();
	size_t width{0};
	size_t height{0};
	uint8_t* data{nullptr};
};

Image load_image(const std::string& path);
