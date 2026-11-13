#include "terrain.h"
#include "importer.h"

void make_terrain_tri(std::vector<VertexData>& verts, std::vector<GLuint>& indices, GLuint a, GLuint b, GLuint c);

Mesh create_terrain_mesh(std::string img_path, std::string texture_path, size_t cells_w, size_t cells_h, GLfloat cell_size, GLfloat height, GLfloat texture_scale) {
	// dont need flipped image here
	stbi_set_flip_vertically_on_load(0);
	Image noise_img = load_image(img_path);

	// leaving texture as 0 if the texture cant be loaded will simply cause the renderer to fall back to the default texture
	GLuint texture{0};
	if (texture_path != "") {
		Image texture_img = load_image(texture_path);
		if (texture_img.data != nullptr ) {
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_img.width, texture_img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_img.data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}	
	}
	stbi_set_flip_vertically_on_load(1);

	const size_t verts_w = cells_w + 1;
	const size_t verts_h = cells_h + 1;
	const GLfloat offset_x = (cell_size*static_cast<float>(cells_w))/2.f;
	const GLfloat offset_y = (cell_size*static_cast<float>(cells_h))/2.f;

	std::vector<VertexData> verts;
	std::vector<GLuint> indices;
	verts.reserve(verts_w*verts_h);
	indices.reserve(cells_w*cells_h*6);
	
	const GLfloat img_scale_w =  static_cast<GLfloat>(noise_img.width)/static_cast<GLfloat>(verts_w);
	const GLfloat img_scale_h =  static_cast<GLfloat>(noise_img.height)/static_cast<GLfloat>(verts_h);

	for (size_t i = 0; i < verts_w*verts_h; i++) {
		const size_t img_x = static_cast<size_t>(static_cast<float>(i%verts_w)*img_scale_w);
		const size_t img_y = static_cast<size_t>(static_cast<float>(i/verts_w)*img_scale_h);
		const GLfloat noise_val = static_cast<GLfloat>(noise_img.data[4*(img_y*noise_img.width+img_x)])/255.f;
		verts.emplace_back(
			static_cast<GLfloat>(i%verts_w)*cell_size - offset_x,
			noise_val * height,
			static_cast<GLfloat>(i/verts_w)*cell_size - offset_y,
			0.f,
			0.f,
			0.f,
			static_cast<GLfloat>(i%verts_w)/static_cast<GLfloat>(verts_w)*(1.f/texture_scale),
			static_cast<GLfloat>(i/verts_w)/static_cast<GLfloat>(verts_h)*(1.f/texture_scale)
		);
	}

	for (size_t i = 0; i < verts_w*verts_h; i++) {
		if ((i%verts_w < cells_w) && (i/verts_w < cells_h)) {
			if ((i/verts_w)%2 != (i%verts_w)%2) {
				make_terrain_tri(verts, indices, i, i+verts_w, i+1);
				make_terrain_tri(verts, indices, i+1, i+verts_w, i+verts_w+1);
			} else {
				make_terrain_tri(verts, indices, i, i+verts_w, i+verts_w+1);
				make_terrain_tri(verts, indices, i, i+verts_w+1, i+1);
			}
		}
		verts[i].set_normal(glm::normalize(verts[i].normal()));
	}

	Material mat = {
		.specular = 0.f,
		.diffuse_texture = texture
	};
	
	return Mesh{verts, indices, {{
		.element_count = static_cast<GLsizei>(indices.size()),
		.material = mat
	}}};
}

void make_terrain_tri(std::vector<VertexData>& verts, std::vector<GLuint>& indices, GLuint a, GLuint b, GLuint c) {
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	const glm::vec3 edge1 = verts[b].position() - verts[a].position();
	const glm::vec3 edge2 = verts[c].position() - verts[a].position();
	const glm::vec3 normal = glm::cross(edge1, edge2);
	verts[a].set_normal(verts[a].normal() + normal);
	verts[b].set_normal(verts[b].normal() + normal);
	verts[c].set_normal(verts[c].normal() + normal);
}

