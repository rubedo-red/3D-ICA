#include "renderer.h"
#include "importer.h"
#include "terrain.h"
#include "freecamera.h"

#include <algorithm>

GLFWwindow* CreateGLFWWindow(int width, int height, const std::string& title);
void DefineGUI();
void resize_window(GLFWwindow* window, int width, int height);

bool viewport_dirty{false};
float aspect_ratio{1.f};

bool is_wireframe{false};

int main()
{
	GLFWwindow* window{ CreateGLFWWindow(960, 960, "3DG") };
	if (!window)
		return -1;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	
	// Renderer should be created first to establish some global state
	Renderer renderer = Renderer();

	Mesh m = model_to_mesh("Data/Models/Mummy/mummy.x", "Data/Models/Mummy/mummy.bmp");
	m.transform.translate = {-0.4f, -0.5f, -5.f};
	m.transform.yaw = 3.14159256f*1.5f;
	m.transform.scale = {0.4f, 0.4f, 0.4f};

	Mesh m2 = model_to_mesh("Data/Models/Jeep/jeep.obj", "Data/Models/Jeep/jeep_army.jpg");
	m2.transform.translate = {0.f, -0.5f, -5.f};
	m2.transform.scale = {0.004f, 0.004f, 0.004f};

	Mesh m3 = model_to_mesh("Data/Models/Jeep/jeep.obj", "Data/Models/Jeep/jeep_rood.jpg");
	m3.transform.translate = {2.5f, 0.2f, -5.f};
	m3.transform.pitch = 3.14159256f*-1.2f;
	m3.transform.roll = 3.14159256f*0.5f;
	m3.transform.yaw = 3.14159256f*0.5f;
	m3.transform.scale = {0.004f, 0.004f, 0.004f};

	Mesh terrain = create_terrain_mesh("Data/Heightmaps/perlin.png", 98, 100, 0.2f, 4.f);
	terrain.transform.translate = {0.f, -6.f, 0.f};

	Transform cube;
	cube.translate = {0.f, 1.f, -8.f};
	cube.scale = {0.5f, 1.0f, 0.5f};

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
	
		cube.yaw += dt * 0.7f;
		cube.pitch += dt * 0.2f;

		renderer.set_wireframe(is_wireframe);

		renderer.clear();
	
		renderer.render_skybox(skybox, view_matrix);
		renderer.render_mesh(m, view_matrix);
		renderer.render_mesh(m2, view_matrix);
		renderer.render_mesh(m3, view_matrix);
		renderer.render_mesh(terrain, view_matrix);

		renderer.render_cube(cube, view_matrix);

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
		ImGui::Checkbox("Wireframe", &is_wireframe);
		ImGui::End();
	}
}

void resize_window(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	aspect_ratio = (float)width / (float)height;
	viewport_dirty = true;
}
