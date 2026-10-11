#include "mesh.h"

void Mesh::process_submesh(aiScene *scene, aiNode *node, std::vector<Submesh>& list, aiMatrix4x4 *p_trans) {
	aiMatrix4x4 trans = (*p_trans) * node->mTransformation;
	for (size_t i = 0; i < node->mNumChildren; i++) {
		process_submesh(scene, node->mChildren[i], list, &trans);
	}
	for (size_t i = 0; i < node->mNumMeshes; i++) {
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
				s.indices.push_back(face.mIndices[ind]);
			}
		}
		list.push_back(s);
	}
}

Mesh::Mesh(aiScene *scene) {
}

Mesh::~Mesh() {
}

void Mesh::render(glm::vec3 project) {
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
	glBufferData(GL_ARRAY_BUFFER, num_verts, verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices, indices, GL_STATIC_DRAW);

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
