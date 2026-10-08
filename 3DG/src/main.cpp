#include "Keith_Helpers.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);
void Render(GLFWwindow* window);
void CreateRectangle();
void CreateShaders();
void DefineGUI();

GLuint gRectangleVAO{ 0 };
GLuint gRectangleEBO{ 0 };
GLuint gShaderProgram{ 0 };
GLuint gTransformLoc{ 0 };

int main()
{
	GLFWwindow* window{ CreateGLFWWindow(1280, 720, "3DG") };
	if (!window)
		return -1;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	CreateRectangle();
	CreateShaders();


	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		Render(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(gShaderProgram);
	glDeleteVertexArrays(1, &gRectangleVAO);
	glDeleteBuffers(0, &gRectangleEBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title)
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialise GLFW" << std::endl;
		return nullptr;
	}

	std::cout << "GLFW initialised" << std::endl;

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 0);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

	glEnable(GL_MULTISAMPLE);

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	GLFWwindow* window{ glfwCreateWindow(width, height, title.c_str(), NULL, NULL) };
	if (!window)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glEnable(GL_MULTISAMPLE);
	glewExperimental = true; 
	std::cout << "GLFW window initialised" << std::endl;

	GLenum err{ glewInit() };
	if (GLEW_OK != err)
	{
		auto errs = glewGetErrorString(err);
		std::cout << "Failed to initialise GLEW, Error: " << errs << std::endl;
		return nullptr;
	}

	std::cout << "GLEW initialised" << std::endl;

	// Enable trapping of OpenGL errors via a callback
	// the callback is in KeithHelpers.cpp
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

	// Disable V-Sync
	glfwSwapInterval(0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	std::cout << "IMGUI initialised" << std::endl;

	return window;
}



void CreateRectangle()
{
	struct vec3 {
		GLfloat x = 0.f;
		GLfloat y = 0.f;
		GLfloat z = 0.f;
	};

	static constexpr vec3 vertices[] = {
		{0.5f, -0.5f, 0.0f}, {1.f, 0.f, 0.f},
		{-0.5f, -0.5f, 0.0f}, {0.f, 1.f, 0.f},
		{-0.5f, 0.5f, 0.0f}, {0.f, 0.f, 1.f},
		{0.5f, 0.5f, 0.0f}, {1.f, 1.f, 1.f}
	};

	static constexpr GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 6,
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glGenBuffers(1, &gRectangleEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gRectangleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &gRectangleVAO);
	glBindVertexArray(gRectangleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), (void*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CreateShaders()
{
	GLuint vertexShader = KeithHelpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert");
	GLuint fragmentShader = KeithHelpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag");
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, vertexShader);
	glAttachShader(gShaderProgram, fragmentShader);


	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	KeithHelpers::LinkProgramShaders(gShaderProgram);
}

void Render(GLFWwindow* window)
{
	glClearColor(0.1f, 0.1f, 0.15f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);	
	
	glUseProgram(gShaderProgram);
	gTransformLoc = glGetUniformLocation(gShaderProgram, "transform");
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, (float)glfwGetTime()*1.f, glm::vec3(0,0,1));
	glUniformMatrix4fv(gTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glBindVertexArray(gRectangleVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gRectangleEBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DefineGUI();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DefineGUI()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	{
		ImGui::Begin("3DG");
		ImGui::Text("Visibility.");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
