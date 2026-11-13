#include "importer.h"
#include <unordered_map>
#include <algorithm>

glm::mat4 ai_mat4_to_glm(aiMatrix4x4t<float> mat) {
	glm::mat4 new_mat;
	new_mat[0][0] = mat.a1; new_mat[1][0] = mat.a2; new_mat[2][0] = mat.a3; new_mat[3][0] = mat.a4;
	new_mat[0][1] = mat.b1; new_mat[1][1] = mat.b2; new_mat[2][1] = mat.b3; new_mat[3][1] = mat.b4;
	new_mat[0][2] = mat.c1; new_mat[1][2] = mat.c2; new_mat[2][2] = mat.c3; new_mat[3][2] = mat.c4;
	new_mat[0][3] = mat.d1; new_mat[1][3] = mat.d2; new_mat[2][3] = mat.d3; new_mat[3][3] = mat.d4;
	return new_mat;
}

Image load_image(const std::string& path) {
	uint8_t* data;
	int width, height, channels;
	data = (uint8_t*)stbi_load(path.c_str(), &width, &height, &channels, 4);
	return {
		.width = static_cast<size_t>(width),
		.height = static_cast<size_t>(height),
		.data = data
	};
}

Image::~Image() {
	stbi_image_free(data);
}

Mesh model_to_mesh(const std::string& model) {
	std::cerr << "loading model: " << model << std::endl;
	std::filesystem::path model_path(model);
	if (!std::filesystem::exists(model)) {
		// TODO: add code to return a builtin placeholder/error model
	};
	

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(model, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	if (scene == nullptr) {
		std::cout << "assimp error: " << importer.GetErrorString() << std::endl;
	};


	aiMatrix4x4 trans;
	std::vector<Material> materials;
	materials.reserve(scene->mNumMaterials);
	std::vector<SubmeshInfo> submeshes;
	submeshes.reserve(scene->mNumMeshes);
	std::vector<VertexData> verts;
	std::vector<GLuint> indices;


	for (size_t i{0}; i < scene->mNumMaterials; i++) {
		auto& asset_material = *scene->mMaterials[i];
		aiColor3D diffuse;
		aiColor3D specular;
		if (AI_SUCCESS != asset_material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) diffuse = {1.f, 1.f, 1.f};
		if (AI_SUCCESS != asset_material.Get(AI_MATKEY_COLOR_SPECULAR, specular)) specular = {0.4f, 0.4f, 0.4f}; 
		
		aiString diffuse_texture_path;
		aiString specular_texture_path;
		aiString normal_texture_path;
		GLuint diffuse_texture{0};
		GLuint specular_texture{0};
		GLuint normal_texture{0};

		std::filesystem::path parent_path = model_path.parent_path();

		if (AI_SUCCESS == asset_material.GetTexture(
			aiTextureType_DIFFUSE, 
			0,
			&diffuse_texture_path,
			NULL, NULL, NULL, NULL, NULL // we don't care about these ( for now )
		)) {
			diffuse_texture = texture_from_ai(scene, diffuse_texture_path.C_Str(), parent_path);
		}
		if (AI_SUCCESS == asset_material.GetTexture(
			aiTextureType_SPECULAR, 
			0,
			&specular_texture_path,
			NULL, NULL, NULL, NULL, NULL
		)) {
			specular_texture = texture_from_ai(scene, specular_texture_path.C_Str(), parent_path);
		}
		if (AI_SUCCESS == asset_material.GetTexture(
			aiTextureType_NORMALS, 
			0,
			&normal_texture_path,
			NULL, NULL, NULL, NULL, NULL
		)) {
			normal_texture = texture_from_ai(scene, normal_texture_path.C_Str(), parent_path);
		}

		Material material = {
			.specular = std::max(std::max(specular.r, specular.g), specular.b),
			.diffuse = { diffuse.r, diffuse.g, diffuse.b },
			.diffuse_texture = diffuse_texture,
			.specular_texture = specular_texture,
			.normal_texture = normal_texture
		};

		materials.push_back(material);
	}
	for (size_t i{0}; i < scene->mNumMeshes; i++) {
		SubmeshInfo mesh_info;
		const auto& mesh = *scene->mMeshes[i];
		mesh_info.start_index = indices.size();
		mesh_info.material = materials[mesh.mMaterialIndex];
		for (size_t f{0}; f < mesh.mNumFaces; f++) {
			const auto& face = mesh.mFaces[f];
			for (size_t ind{0}; ind<face.mNumIndices; ind++) {
				indices.push_back(verts.size() + face.mIndices[ind]);
				mesh_info.element_count++;
			}
		}
		for (size_t v{0}; v < mesh.mNumVertices; v++) {
			const auto& vert = mesh.mVertices[v];
			const auto& norm = mesh.mNormals[v];
			aiVector3D uv(0.f, 0.f, 0.f);
			if (mesh.GetNumUVChannels() > 0) uv = mesh.mTextureCoords[0][v];
			verts.push_back(VertexData{
				.x  = vert.x,
				.y  = vert.y,
				.z  = vert.z,
				.nx = norm.x,
				.ny = norm.y,
				.nz = norm.z,
				.u  = uv.x,
				.v  = uv.y,
			});
		}
		submeshes.push_back(mesh_info);
	}

	std::vector<SubmeshInfo> parts;
	visit_ai_node(scene->mRootNode, submeshes, parts);

	return Mesh(verts, indices, parts);
}

GLuint texture_from_ai(const aiScene *scene, std::filesystem::path texture_path, std::filesystem::path parent_path) {
	stbi_set_flip_vertically_on_load(1);
	GLuint* data{nullptr};
	int width, height, channels;
	if (const aiTexture* embedded = scene->GetEmbeddedTexture(texture_path.c_str())) {
		// if height is 0, embedded texture needs to be decoded
		if (embedded->mHeight == 0) 
			data = (GLuint*)stbi_load_from_memory((GLubyte*)embedded->pcData, embedded->mWidth, &width, &height, &channels, 4);
		else {
			data = (GLuint*)embedded->pcData;
			width = embedded->mWidth;
			height = embedded->mHeight;
		}
	} else {
		if (!std::filesystem::exists(parent_path/texture_path)) return 0;
		data = (GLuint*)stbi_load((parent_path/texture_path).c_str(), &width, &height, &channels, 4);
	}
	if (data == nullptr) {
		// if image loading failed, use default texture instead
		std::cout << stbi_failure_reason() << std::endl;
		// 0 will tell renderer to use default texture
		return 0;
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

void visit_ai_node(aiNode *node, std::vector<SubmeshInfo>& unique_meshes, std::vector<SubmeshInfo>& parts, glm::mat4 p_trans) {
	glm::mat4 trans = p_trans * ai_mat4_to_glm(node->mTransformation);
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		SubmeshInfo m = unique_meshes[node->mMeshes[i]];
		m.transform = trans;
		parts.push_back(m);
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		visit_ai_node(node->mChildren[i], unique_meshes, parts, trans);
	}
}
