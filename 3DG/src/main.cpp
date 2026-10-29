#include "renderer.h"
#include "importer.h"
#include "freecamera.h"

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
	
	// Renderer should be created first to establish some global state
	Renderer renderer = Renderer();

	Mesh m = model_to_mesh("Data/Models/Mummy/mummy.x", "Data/Models/Mummy/mummy.bmp");
	m.transform = glm::translate(m.transform, {-0.4f, -0.5f, -5.f});
	m.transform = glm::rotate(m.transform, 3.14159256f*1.5f, {0.f, 1.f, 0.f});
	m.transform = glm::scale(m.transform, {0.4f, 0.4f, 0.4f});

	Mesh m2 = model_to_mesh("Data/Models/Jeep/jeep.obj", "Data/Models/Jeep/jeep_army.jpg");
	m2.transform = glm::translate(m2.transform, {0.f, -0.5f, -5.f});
	m2.transform = glm::scale(m2.transform, {0.004f, 0.004f, 0.004f});

	Mesh m3 = model_to_mesh("Data/Models/Jeep/jeep.obj", "Data/Models/Jeep/jeep_rood.jpg");
	m3.transform = glm::translate(m3.transform, {2.5f, 0.2f, -5.f});
	m3.transform = glm::rotate(m3.transform, 3.14159256f*0.3f, {0.f, 1.f, 0.f});
	m3.transform = glm::rotate(m3.transform, 3.14159256f*0.5f, {1.f, 0.f, 0.f});
	m3.transform = glm::scale(m3.transform, {0.004f, 0.004f, 0.004f});

	//Mesh m4 = model_to_mesh("Data/Models/Test/satori.glb");
	//m4.transform = glm::translate(m4.transform, {3.f, -0.2f, -4.f});
	//m4.transform = glm::rotate(m4.transform, 3.14159256f*0.3f, {0.f, 1.f, 0.f});
	//m4.transform = glm::rotate(m4.transform, 3.14159256f*0.5f, {1.f, 0.f, 0.f});

	//Mesh m5 = model_to_mesh("Data/Models/Test/aya.glb");
	//m5.transform = glm::translate(m5.transform, {1.5f, -0.5f, -2.f});

	Skybox skybox(
		"Data/Models/Sky/Clouds/SkyBox_Right.tga",
		"Data/Models/Sky/Clouds/SkyBox_Left.tga",
		"Data/Models/Sky/Clouds/SkyBox_Top.tga",
		"Data/Models/Sky/Clouds/SkyBox_Bottom.tga",
		"Data/Models/Sky/Clouds/SkyBox_Back.tga",
		"Data/Models/Sky/Clouds/SkyBox_Front.tga"
	);

	FreeCamera camera;

	double prev_time = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float dt = static_cast<float>(glfwGetTime() - prev_time);
		prev_time = glfwGetTime();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
			camera.rotate_pitch(false, dt);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
			camera.rotate_pitch(true, dt);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
			camera.rotate_yaw(false, dt);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.rotate_yaw(true, dt);

		if (camera.dirty_movement_vectors)
			camera.recalculate_movement_vectors();

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
			camera.move_right(true, dt);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.move_right(false, dt);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.move_forward(false, dt);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.move_forward(true, dt);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.move_up(false, dt);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.move_up(true, dt);

		if (viewport_dirty) {
			renderer.set_projection(glm::radians(50.f), aspect_ratio, 0.01f, 1000.f);
			viewport_dirty = false;
		}
		const glm::mat4 view_matrix = camera.camera.calculate_view_matrix();
	
		//m4.transform = glm::rotate(m4.transform, 3.14159256f*dt, {0.f, 1.f, 0.f});

		renderer.clear();
	
		renderer.render_skybox(skybox, view_matrix);
		renderer.render_mesh(m, view_matrix);
		renderer.render_mesh(m2, view_matrix);
		renderer.render_mesh(m3, view_matrix);
		//renderer.render_mesh(m4, view_matrix);
		//renderer.render_mesh(m5, view_matrix);

		renderer.render_cube(glm::mat4(1), view_matrix);

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
