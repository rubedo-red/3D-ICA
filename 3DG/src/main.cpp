#include "renderer.h"
#include "camera.h"

#include <algorithm>

GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);
void DefineGUI();
void resize_window(GLFWwindow* window, int width, int height);

bool viewport_dirty{false};
float aspect_ratio{1.f};

int main()
{
	GLFWwindow* window{ CreateGLFWWindow(960, 960, "3DG") };
	if (!window)
		return -1;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	Mesh m("Data/Models/Test/satori.glb");
	m.transform = glm::translate(m.transform, {0.f, -0.5f, -5.f});
	Mesh m2("Data/Models/Test/koishi.glb");
	m2.transform = glm::translate(m2.transform, {1.f, -0.5f, -5.f});
	Mesh m3("Data/Models/Test/remilia.glb");
	m3.transform = glm::translate(m3.transform, {-1.f, -0.5f, -5.f});
	Mesh m4("Data/Models/Test/aya.glb");
	m4.transform = glm::translate(m4.transform, {-2.f, -0.5f, -5.f});
	Mesh m5("Data/Models/Mummy/mummy.x", "Data/Models/Mummy/mummy.bmp");
	m5.transform = glm::translate(m5.transform, {-2.f, -0.5f, -7.f});

	Renderer renderer = Renderer();
	Camera camera;
	bool camera_dirty = 0;
	float camera_speed = 0.05;

	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.pitch =  std::clamp(camera.pitch + 0.035f, -1.570796f, 1.570795f);
			camera_dirty |= 1;
		} 
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.pitch =  std::clamp(camera.pitch - 0.035f, -1.570796f, 1.570795f);
			camera_dirty |= 1;
		} 
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.yaw =  std::fmod(camera.yaw + 0.035f, 6.2831853f);
			camera_dirty |= 1;
		} 
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.yaw =  std::fmod(camera.yaw - 0.035f, 6.2831853f);
			camera_dirty |= 1;
		} 
		if (camera_dirty)
			camera.recalculate_movement_vectors();
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
			camera.position += camera.right_vec * -camera_speed;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.position += camera.right_vec * camera_speed;
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.position += camera.forward_vec * -camera_speed;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.position += camera.forward_vec * camera_speed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.position += camera.up_vec * camera_speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.position += camera.up_vec * -camera_speed;

		if (viewport_dirty) {
			renderer.set_projection(glm::radians(50.f), aspect_ratio, 0.01f, 1000.f);
			viewport_dirty = false;
		}
		renderer.clear();
		renderer.render_mesh(m, camera.calculate_view_matrix());
		renderer.render_mesh(m2, camera.calculate_view_matrix());
		renderer.render_mesh(m3, camera.calculate_view_matrix());
		renderer.render_mesh(m4, camera.calculate_view_matrix());
		renderer.render_mesh(m5, camera.calculate_view_matrix());

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

	glfwSetWindowSizeCallback(window, &resize_window);

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

void resize_window(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	aspect_ratio = (float)width / (float)height;
	viewport_dirty = true;
}
