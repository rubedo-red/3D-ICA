#include "terrain.h"
#include "importer.h"

Mesh create_terrain_mesh(std::string img_path, size_t cells_w, size_t cells_h, GLfloat cell_size, GLfloat height) {
	// dont need flipped image here
	stbi_set_flip_vertically_on_load(0);
	Image noise_img = load_image(img_path);
	stbi_set_flip_vertically_on_load(1);

	std::vector<VertexData> verts;
	std::vector<GLuint> indices;
	
	const size_t verts_w = cells_w + 1;
	const size_t verts_h = cells_h + 1;
	const GLfloat offset_x = (cell_size*static_cast<float>(cells_w))/2.f;
	const GLfloat offset_y = (cell_size*static_cast<float>(cells_h))/2.f;
	
	const GLfloat img_scale_w =  static_cast<GLfloat>(noise_img.width)/static_cast<GLfloat>(verts_w);
	const GLfloat img_scale_h =  static_cast<GLfloat>(noise_img.height)/static_cast<GLfloat>(verts_h);

	for (size_t i = 0; i < verts_w*verts_h; i++) {
		const size_t img_x = static_cast<size_t>(static_cast<float>(i%verts_w)*img_scale_w);
		const size_t img_y = static_cast<size_t>(static_cast<float>(i/verts_w)*img_scale_h);
		const GLfloat noise_val = static_cast<GLfloat>(noise_img.data[4*(img_y*noise_img.width+img_x)])/255.f;
		verts.push_back({
			.x = static_cast<GLfloat>(i%verts_w)*cell_size - offset_x,
			.y = noise_val * height,
			.z = static_cast<GLfloat>(i/verts_w)*cell_size - offset_y
		});

		if ((i%verts_w < cells_w) && (i/verts_w < cells_h)) {
			if ((i/verts_w)%2 != (i%verts_w)%2) {
				indices.push_back(i);
				indices.push_back(i+verts_w);
				indices.push_back(i+1);
				indices.push_back(i+1);
				indices.push_back(i+verts_w);
				indices.push_back(i+verts_w+1);
			} else {
				indices.push_back(i);
				indices.push_back(i+verts_w);
				indices.push_back(i+verts_w+1);
				indices.push_back(i);
				indices.push_back(i+verts_w+1);
				indices.push_back(i+1);
			}
		}
	}

	return Mesh{verts, indices, {{indices.size(), 0}}};
}
