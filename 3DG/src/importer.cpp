#include "importer.h"
#include <unordered_map>
#include <algorithm>

GLuint default_texture{0};

struct Submesh {
	std::string texture{""};
	GLuint verts_begin{0};
	GLuint indices_begin{0};
	GLuint verts_count{0};
	GLuint indices_count{0};
};

void create_default_texture() {
	stbi_set_flip_vertically_on_load(1);
	static constexpr GLubyte default_texture_data[] = {
		255, 255, 255
	};
	// this must live for the entire duration of the program, so we dont need to worry about deletion
	glGenTextures(1, &default_texture);
	glBindTexture(GL_TEXTURE_2D, default_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, default_texture_data);
}

Mesh model_to_mesh(const std::string& model, const std::string& default_override) {
	if (default_texture == 0) create_default_texture();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene == nullptr) {
		std::cout << "assimp error: " << importer.GetErrorString() << std::endl;
	};

	aiMatrix4x4 trans;
	std::vector<Submesh> submeshes;
	std::vector<VertexData> verts;
	std::vector<GLuint> indices;
	visit_ai_node(scene, scene->mRootNode, submeshes, verts, indices, trans);

	std::unordered_map<std::string, GLuint> textures_map;
	std::vector<std::pair<size_t, GLuint>> texture_areas;

	const bool has_override = default_override != "";
	for (auto& s : submeshes) { 
		if (has_override && s.texture == "") 
			s.texture = default_override; 
		if (!textures_map.contains(s.texture) && s.texture != "") {
			textures_map[s.texture] = texture_from_ai(scene, s.texture);
		}
		texture_areas.push_back({s.indices_count, textures_map[s.texture]});
	};

	return Mesh(verts, indices, texture_areas);
}

GLuint texture_from_ai(const aiScene *scene, std::string texture_name) {
	GLuint* data{nullptr};
	int width, height, channels;
	if (const aiTexture* embedded = scene->GetEmbeddedTexture(texture_name.c_str())) {
		// if height is 0, embedded texture needs to be decoded
		if (embedded->mHeight == 0) 
			data = (GLuint*)stbi_load_from_memory((GLubyte*)embedded->pcData, embedded->mWidth, &width, &height, &channels, 4);
		else {
			data = (GLuint*)embedded->pcData;
			width = embedded->mWidth;
			height = embedded->mHeight;
		}
	} else {
		data = (GLuint*)stbi_load(texture_name.c_str(), &width, &height, &channels, 4);
	}
	if (data == nullptr) {
		// if image loading failed, use default texture instead
		std::cout << stbi_failure_reason() << std::endl;
		return default_texture;
	}
	GLuint gltext{0};
	glGenTextures(1, &gltext);
	glBindTexture(GL_TEXTURE_2D, gltext);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return gltext;
}

void visit_ai_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& submeshes, std::vector<VertexData>& vertices, std::vector<GLuint>& indices, aiMatrix4x4& p_trans) {
	aiMatrix4x4 trans = p_trans * node->mTransformation;
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		GLuint base_vert = vertices.size();
		GLuint next_base = base_vert;
		Submesh s;
		s.verts_begin = vertices.size();
		s.indices_begin = indices.size();
		aiMesh *m = scene->mMeshes[node->mMeshes[i]];
		aiMaterial* material = scene->mMaterials[m->mMaterialIndex];
		const bool has_text_coords = m->HasTextureCoords(0);
		const size_t t_count = material->GetTextureCount(aiTextureType_BASE_COLOR);
		if (t_count > 0) {
			aiString path;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
			s.texture = std::string(path.C_Str());
		} 
		for (size_t j = 0; j < m->mNumVertices; j++) {
			next_base++;
			VertexData v_data;
			aiVector3D v = trans * (m->mVertices[j]);
			aiVector3D n = trans * (m->mNormals[j]);
			v_data.x = v.x;
			v_data.y = v.y;
			v_data.z = v.z;
			v_data.nx = n.x;
			v_data.ny = n.y;
			v_data.nz = n.z;
			if (has_text_coords) {
				aiVector3D t = (m->mTextureCoords[0][j]);
				v_data.u = t.x;
				v_data.v = t.y;
			}
			vertices.push_back(v_data);
		}
		s.verts_count = next_base;
		for (size_t j = 0; j < m->mNumFaces; j++) {
			aiFace face = m->mFaces[j];
			for (size_t ind = 0; ind < face.mNumIndices; ind++) {
				s.indices_count++;
				indices.push_back(face.mIndices[ind]+base_vert);
			}
		}
		base_vert = next_base;
		submeshes.push_back(s);
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		visit_ai_node(scene, node->mChildren[i], submeshes, vertices, indices, trans);
	}
}
