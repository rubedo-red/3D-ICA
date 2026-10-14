#include "renderer.h"

Renderer::Renderer(GLFWwindow* window) {
	set_default_flags();
	init_imgui(window);
	create_mesh_shader();
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

void Renderer::init_imgui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	std::cout << "IMGUI initialised" << std::endl;
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

void Renderer::clear() {
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render_mesh(Mesh& mesh) {
	glUseProgram(mesh_shader);
	GLuint transform_loc = glGetUniformLocation(mesh_shader, "transform");
	glm::mat4 trans = mesh.transform;
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
}
