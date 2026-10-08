/*
	3D Graphics Start Point

	I have tried to provide the minimum amount of code to get you started. The KeithHelpers file
	provides some useful functions for loading and compiling shaders, loading files into strings etc.
	Feel free to replace this with your own code if you prefer and please check it over to see what it does.

	There are a number of libraries included:

	GLFW: https://www.glfw.org/ - this provides a simple API for creating windows,
	GLEW: http://glew.sourceforge.net/ - The OpenGL Extension Wrangler Library.
	GLM: https://glm.g-truc.net/0.9.9/index.html - OpenGL Mathematics, a header only C++ mathematics library for graphics software.
	FreeImage: http://freeimage.sourceforge.net/ - used to load image files
	Assimp: https://www.assimp.org/ - A 3D model loader library
	IMGUI: https://github.com/ocornut/imgui - a bloat-free graphical user interface library for C++

	Jamie (September 2026)
*/

// Keith Helpers (I am keeping the name this year!) - also includes headers needed for the libraries used
#include "Keith_Helpers.h"


GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);
void Render(GLFWwindow* window);
void CreateRectangle();
void CreateShaders();
void DefineGUI();

// Since this is a simple demo we will use some globals
// A real program should use a class to encapsulate state
GLuint gTriangleVAO{ 0 };
GLuint gTriangleEBO{ 0 };
GLuint gShaderProgram{ 0 };

int main()
{
	GLFWwindow* window{ CreateGLFWWindow(1280, 720, "3DG") };
	if (!window)
		return -1;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	CreateRectangle();
	CreateShaders();

	// Enter main loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		Render(window);

		// GLFW updating - internally swaps the front and back buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// You should delete any OpenGL resources here
	glDeleteProgram(gShaderProgram);
	glDeleteVertexArrays(1, &gTriangleVAO);

	// Clean up and exit
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// Uses GLFW to set up a window via GLFW. Also initialises GLEW and OpenGL.
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.6 minimum
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

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

	glewExperimental = true; // Needed in core profile

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

	// Disable V-Sync for speed
	glfwSwapInterval(0);

	// Setup Dear ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style and Platform/Renderer bindings
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	std::cout << "IMGUI initialised" << std::endl;

	return window;
}

void CreateRectangle()
{
	static constexpr GLfloat vertices[] = {
		0.5f, -0.5f, 0.0f, 1.f, 0.f, 0.f,
		-0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f,
		-0.5f, 0.5f, 0.0f, 0.f, 0.f, 1.f,
		0.5f, 0.5f, 0.0f, 1.f, 1.f, 1.f
	};

	static constexpr GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// TODO: create a triangle
	
	// Remember the process for OpenGL objects:
	// 1. Create the object id e.g. glGenBuffers(1, &bufferID);
	// 2. Bind the object e.g. glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	// 3. Set any parameters e.g. glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	// 4. Unbind the object e.g. glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glGenBuffers(1, &gTriangleEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTriangleEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &gTriangleVAO);
	glBindVertexArray(gTriangleVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CreateShaders()
{
	GLuint vertexShader = KeithHelpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.vert");
	GLuint fragmentShader = KeithHelpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.frag");
	gShaderProgram = glCreateProgram();

	// Attach the vertex shader to this program (copies it)
	glAttachShader(gShaderProgram, vertexShader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(gShaderProgram, 0, "vertex_position");
	//glBindAttribLocation(gShaderProgram, 1, "vertex_color");

	// Attach the fragment shader (copies it)
	glAttachShader(gShaderProgram, fragmentShader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	KeithHelpers::LinkProgramShaders(gShaderProgram);
}

void Render(GLFWwindow* window)
{
	// Clear the screen each time
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);	
	
	// Use the shader program
	glUseProgram(gShaderProgram);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTriangleEBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	// IMGUI	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DefineGUI();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	{
		ImGui::Begin("3DG");                    // Create a window called "3GP" and append into it.

		ImGui::Text("Visibility.");             // Display some text (you can use a format strings too)	

	//	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}
}
