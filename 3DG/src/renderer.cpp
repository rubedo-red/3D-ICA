#include "renderer.h"

Renderer::Renderer() {
	set_default_flags();
	create_mesh_shader();
	set_projection(glm::radians(50.f), 1.f, 0.01f, 1000.f);
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

void Renderer::create_mesh_shader() {
	GLuint vertexShader = KeithHelpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert");
	GLuint fragmentShader = KeithHelpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag");
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
	glClearColor(0.1f, 0.1f, 0.15f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render_mesh(const Mesh& mesh, const glm::mat4& view_matrix) {
	glUseProgram(mesh_shader);
	GLuint transform_loc = glGetUniformLocation(mesh_shader, "transform");
	glm::mat4 trans = projection_matrix * view_matrix * mesh.transform;
	glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(trans));
	size_t current = 0;
	glBindVertexArray(mesh.VAO);
	for (auto& [n, t] : mesh.submesh_textures) {
		glBindTexture(GL_TEXTURE_2D, t);
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, (void*)(current*sizeof(GLuint)));
		current += n;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer() {
	glDeleteProgram(mesh_shader);
}
