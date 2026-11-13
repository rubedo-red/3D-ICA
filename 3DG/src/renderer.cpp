#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Renderer::Renderer() :
	cube_primitive(cube_data, sizeof(cube_data)/sizeof(VertexData), cube_indices, sizeof(cube_indices)/sizeof(GLuint), {{
		.element_count = sizeof(cube_indices)/sizeof(GLuint)
	}})
{
	set_default_flags();
	create_mesh_shader();
	create_skybox();
	create_skybox_shader();
	if (default_texture == 0) create_default_texture();
	set_projection(glm::radians(50.f), 1.f, 0.15f, 1000.f);
}

void Renderer::set_wireframe(bool enable) {
	glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
}

void Renderer::set_lighting_enable(bool enable) {
	glUseProgram(mesh_shader);
	glUniform1i(glGetUniformLocation(mesh_shader, "light_enable"), enable);
}

void Renderer::create_skybox() {
	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);
	glGenBuffers(1, &skybox_ebo);
	glBindVertexArray(skybox_vao);

	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skybox_indices), skybox_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::set_default_flags() {
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
#if defined(_DEBUG)			
	int flags{ 0 };
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		// initialize debug output 
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(KeithHelpers::glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif
}

void Renderer::create_skybox_shader() {
	GLuint vertexShader = KeithHelpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/skybox_shader.vert");
	GLuint fragmentShader = KeithHelpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/skybox_shader.frag");
	skybox_shader = glCreateProgram();
	glAttachShader(skybox_shader, vertexShader);
	glAttachShader(skybox_shader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	KeithHelpers::LinkProgramShaders(skybox_shader);
}

void Renderer::create_mesh_shader() {
	GLuint vertexShader = KeithHelpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/mesh_shader.vert");
	GLuint fragmentShader = KeithHelpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/mesh_shader.frag");
	mesh_shader = glCreateProgram();
	glAttachShader(mesh_shader, vertexShader);
	glAttachShader(mesh_shader, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	KeithHelpers::LinkProgramShaders(mesh_shader);
}

void Renderer::set_projection(float fov_y, float aspect, float near_dist, float far_dist) {
	float fov_x = 2*glm::atan(aspect * glm::tan(fov_y/2));
	projection_matrix = glm::mat4(
		1.f/glm::tan(fov_x/2.f), 0.f, 0.f, 0.f,
		0.f, 1.f/glm::tan(fov_y/2.f), 0.f, 0.f,
		0.f, 0.f, -(far_dist+near_dist)/(far_dist-near_dist), -1.f,
		0.f, 0.f, -2.f*(far_dist*near_dist)/(far_dist-near_dist), 0.f
	);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render_mesh(const Mesh& mesh, const glm::mat4& view_matrix) {
	glUseProgram(mesh_shader);
	GLuint transform_loc = glGetUniformLocation(mesh_shader, "view_transform");
	GLuint mesh_transform_loc = glGetUniformLocation(mesh_shader, "obj_transform");
	GLuint color_loc = glGetUniformLocation(mesh_shader, "obj_color");
	GLuint view_pos_loc = glGetUniformLocation(mesh_shader, "view_pos");
	GLuint specular_loc = glGetUniformLocation(mesh_shader, "specular_value");
	glm::mat4 base_trans = projection_matrix * view_matrix;
	glm::vec3 color = {1.f, 1.f, 1.f};
	glUniformMatrix4fv(mesh_transform_loc, 1, GL_FALSE, glm::value_ptr(mesh.transform.matrix()));
	glBindVertexArray(mesh.VAO);
	for (auto& submesh : mesh.submeshes) {
		glm::mat4 trans = submesh.transform * base_trans;
		glUniform3f(view_pos_loc, trans[3][0], trans[3][1], trans[3][2]);
		glUniform1f(specular_loc, submesh.material.specular);
		glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniform3f(color_loc, submesh.material.diffuse.r, submesh.material.diffuse.g, submesh.material.diffuse.b);
		// if no texture, use 1x1 white texture
		if (submesh.material.diffuse_texture == 0) glBindTexture(GL_TEXTURE_2D, default_texture);
		else glBindTexture(GL_TEXTURE_2D, submesh.material.diffuse_texture);
		glDrawElements(GL_TRIANGLES, submesh.element_count, GL_UNSIGNED_INT, (void*)(submesh.start_index*sizeof(GLuint)));
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::render_skybox(const Skybox& skybox, const glm::mat4& view_matrix) {
	// dont use depth when drawing skybox, it needs to be below the rest of the scene
	glDepthMask(GL_FALSE);

	glUseProgram(skybox_shader);
	glBindVertexArray(skybox_vao);

	GLuint transform_loc = glGetUniformLocation(skybox_shader, "transform");
	glm::mat4 trans = projection_matrix * glm::mat4(glm::mat3(view_matrix));
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(trans));
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDepthMask(GL_TRUE);
}

Renderer::~Renderer() {
	glDeleteProgram(mesh_shader);
	glDeleteProgram(skybox_shader);
	glDeleteBuffers(1, &skybox_vbo);
	glDeleteBuffers(1, &skybox_ebo);
	glDeleteVertexArrays(1, &skybox_vao);
}

void Renderer::render_cube(const Transform& transform, const glm::mat4& view_matrix) {
	//glUseProgram(mesh_shader);
	//GLuint transform_loc = glGetUniformLocation(mesh_shader, "view_transform");
	//GLuint mesh_transform_loc = glGetUniformLocation(mesh_shader, "obj_transform");
	//GLuint color_loc = glGetUniformLocation(mesh_shader, "obj_color");
	//GLuint view_pos_loc = glGetUniformLocation(mesh_shader, "view_pos");
	//glm::mat4 trans = projection_matrix * view_matrix;
	//glUniform3f(view_pos_loc, trans[3][0], trans[3][1], trans[3][2]);
	//glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(trans));
	//glUniformMatrix4fv(mesh_transform_loc, 1, GL_FALSE, glm::value_ptr(transform.matrix()));
	//glBindTexture(GL_TEXTURE_2D, default_texture);
	//glBindVertexArray(cube_primitive.VAO);
	//size_t current = 0;
	//for (size_t i = 0; i < 6; i++) {
	//	glm::vec3 color = primitive_colors[i%(sizeof(primitive_colors)/sizeof(glm::vec3))];
	//	glUniform3f(color_loc, color.r, color.g, color.b);
	//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(current*sizeof(GLuint)));
	//	current += 6;
	//}
	//glUniform3f(color_loc, 1.f, 1.f, 1.f);
	//glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}
