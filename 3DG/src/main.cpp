#include "renderer.h"

GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);
void DefineGUI();

int main()
{
	GLFWwindow* window{ CreateGLFWWindow(960, 960, "3DG") };
	if (!window)
		return -1;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	Mesh m("Data/Models/Test/koishi.glb");
	m.transform = glm::translate(m.transform, {0.f, -0.5f, -5.f});

	Renderer renderer = Renderer();

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {0.05f, 0.f, 0.f});
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {-0.05f, 0.f, 0.f});
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {0.f, 0.f, 0.05f});
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {0.f, 0.f, -0.05f});
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {0.f, -0.05f, 0.f});
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			renderer.view_matrix = glm::translate(renderer.view_matrix, {0.f, 0.05f, 0.f});

		renderer.clear();
		renderer.render_mesh(m);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DefineGUI();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

void DefineGUI()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	{
		ImGui::Begin("3DG");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
