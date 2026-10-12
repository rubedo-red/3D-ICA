#include "mesh.h"

void Mesh::visit_node(const aiScene *scene, aiNode *node, std::vector<Submesh>& list, aiMatrix4x4& p_trans, GLuint base_vert) {
	aiMatrix4x4 trans = p_trans * node->mTransformation;
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		std::cout << "mesh num " << i << std::endl;
		GLuint next_base = base_vert;
		Submesh s;
		aiMesh *m = scene->mMeshes[node->mMeshes[i]];
		aiMaterial* material = scene->mMaterials[m->mMaterialIndex];
		const bool has_text_coords = m->HasTextureCoords(0);
		const size_t t_count = material->GetTextureCount(aiTextureType_BASE_COLOR);
		if (t_count > 0) {
			aiString path;
			material->GetTexture(aiTextureType_BASE_COLOR, 0, &path);
			if (const aiTexture* embedded = scene->GetEmbeddedTexture(path.C_Str())) {
				s.texture = embedded;
			} 
		} 
		for (size_t j = 0; j < m->mNumVertices; j++) {
			next_base++;
			VertexData v_data;
			aiVector3D v = trans * (m->mVertices[j]);
			aiVector3D n = (m->mNormals[j]);
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
			s.vertices.push_back(v_data);
		}
		for (size_t j = 0; j < m->mNumFaces; j++) {
			aiFace face = m->mFaces[j];
			for (size_t ind = 0; ind < face.mNumIndices; ind++) {
				s.indices.push_back(face.mIndices[ind]+base_vert);
			}
		}
		base_vert = next_base;
		list.push_back(s);
	}
	for (size_t i = 0; i < node->mNumChildren; i++) {
		visit_node(scene, node->mChildren[i], list, trans, base_vert);
	}
}

GLuint Mesh::create_gltexture(const aiTexture* texture) {
	if (texture == nullptr) return 0;
	if (texture->mHeight == 0 && texture->mWidth != 0) {
		return 0;
	}
	return 0;
}

Mesh::Mesh(const aiScene *scene) {
	std::vector<Submesh> submeshes;
	aiMatrix4x4 trans;
	visit_node(scene, scene->mRootNode, submeshes, trans);
	std::vector<VertexData> verts;
	std::vector<GLuint> idxs;
	for (auto& s : submeshes) {
		std::cout << "indices in submesh: " << s.indices.size() << std::endl;
		std::cout << "verts in submesh: " << s.vertices.size() << std::endl;
		verts.insert(verts.end(), s.vertices.begin(), s.vertices.end());
		idxs.insert(idxs.end(), s.indices.begin(), s.indices.end());
		textures.push_back({
			s.indices.size(),
			create_gltexture(s.texture)
		});
	}
	create_buffers(verts.data(), verts.size(), idxs.data(), idxs.size());
}

Mesh::~Mesh() {
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::render(glm::vec3 project) {
	glBindVertexArray(VAO);
	size_t current = 0;
	for (auto& t : textures) {
			glDrawElements(GL_TRIANGLES, t.first, GL_UNSIGNED_INT, (void*)(current*sizeof(GLuint)));
		current += t.first;
	}
	glBindVertexArray(0);
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
