#include "mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void Mesh::visit_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& submeshes, std::vector<VertexData>& vertices, std::vector<GLuint>& indices, aiMatrix4x4& p_trans) {
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
		visit_node(scene, node->mChildren[i], submeshes, vertices, indices, trans);
	}
}

GLuint Mesh::create_gltexture(const aiScene *scene, std::string texture) {
	if (default_texture == 0) {
		stbi_set_flip_vertically_on_load(1);
		static constexpr GLubyte default_texture_data[] = {
			255, 255, 255
		};
		// this must live for the entire duration of the program, so we dont need to worry about deletion
		glGenTextures(1, &default_texture);
		glBindTexture(GL_TEXTURE_2D, default_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, default_texture_data);
	}
	if (texture == "") {
		return default_texture;
	}
	int width, height, channels;
	GLuint* data{nullptr};
	if (const aiTexture* embedded = scene->GetEmbeddedTexture(texture.c_str())) {
		// if height is 0, embedded texture needs to be decoded
		if (embedded->mHeight == 0) 
			data = (GLuint*)stbi_load_from_memory((GLubyte*)embedded->pcData, embedded->mWidth, &width, &height, &channels, 4);
		else {
			data = (GLuint*)embedded->pcData;
			width = embedded->mWidth;
			height = embedded->mHeight;
		}
	} else {
		data = (GLuint*)stbi_load(texture.c_str(), &width, &height, &channels, 4);
	}
	if (data == nullptr) {
		// if image loading failed, use default texture instead
		std::cout << stbi_failure_reason() << std::endl;
		return create_gltexture(scene, "");
	}
	GLuint gltext;
	glGenTextures(1, &gltext);
	glBindTexture(GL_TEXTURE_2D, gltext);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return gltext;
}

Mesh::Mesh(const aiScene *scene, std::string t_override) {
	aiMatrix4x4 trans;
	std::vector<Submesh> submeshes;
	std::vector<VertexData> verts;
	std::vector<GLuint> indices;
	visit_node(scene, scene->mRootNode, submeshes, verts, indices, trans);
	create_buffers(verts.data(), verts.size(), indices.data(), indices.size());
	for (auto& s : submeshes) {
		textures.push_back({
			s.indices_count,
			create_gltexture(scene, (s.texture == "" && t_override != "") ? t_override : s.texture)
		});
	}
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::render(glm::vec3 project) {
	size_t current = 0;
	glBindVertexArray(VAO);
	for (auto& t : textures) {
		glBindTexture(GL_TEXTURE_2D, t.second);
		glDrawElements(GL_TRIANGLES, t.first, GL_UNSIGNED_INT, (void*)(current*sizeof(GLuint)));
		current += t.first;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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
