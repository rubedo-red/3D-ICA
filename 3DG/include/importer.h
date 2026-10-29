#pragma once

#include "stb/stb_image.h"
#include "mesh.h"

struct Submesh;

Mesh model_to_mesh(const std::string& model, const std::string& default_override="");

// caller is resposible for ownership of returned texture
GLuint texture_from_ai(const aiScene *scene, std::string texture_name);

void visit_ai_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& submeshes, std::vector<VertexData>& vertices, std::vector<GLuint>& indices, aiMatrix4x4& p_trans);
