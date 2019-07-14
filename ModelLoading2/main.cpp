#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "debug.h"

using namespace std;
namespace fs = std::filesystem;

void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float ZNEAR = 0.01f;
const float ZFAR = 100.f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return 1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader shader("shader.vert", "shader.frag");

	// Load mesh
	// ---------
	Model model1("../../../Resources/models/nanosuit/nanosuit.obj");
	//Model model1(R"(C:\Users\Ryan\3D Objects\hylian_shield\model.dae)");

	//model1.materials[0].diffuse_texture = Texture(R"(C:\Users\Ryan\3D Objects\hylian_shield\textures\02 - Default_albedo.jpg)");
	//model1.materials[0].specular_texture = Texture(R"(C:\Users\Ryan\3D Objects\hylian_shield\textures\specular.jpg)");
	//model1.materials[0].ao_texture = Texture(R"(C:\Users\Ryan\3D Objects\hylian_shield\textures\02 - Default_AO.jpg)");
	//model1.materials[0].diffuse_color = glm::vec3(0.6f);
	//model1.materials[0].specular_color = glm::vec3(0.6f);
	//model1.materials[0].shininess = 15.0f;

	//for (auto& mat : model1.materials) {
	//	cout << mat << endl;
	//}

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = float(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("viewPos", camera.Position);

		glm::vec3 lightColor(1.0);
		//glm::vec3 lightColor = glm::clamp(glm::sin(currentFrame * glm::vec3(2.0f, 0.7f, 1.3f)), 0.0f, 1.0f) * 1.5f;

		// directional light
		shader.setInt("numDirLights", 3);
		shader.setVec3("dirLights[0].direction", -1.0f, -1.0f, -1.0f);
		shader.setVec3("dirLights[0].ambient", 0.2f * lightColor);
		shader.setVec3("dirLights[0].diffuse", lightColor);
		shader.setVec3("dirLights[0].specular", lightColor);

		shader.setVec3("dirLights[1].direction", 1.0f, -1.0f, -1.0f);
		shader.setVec3("dirLights[1].ambient", glm::vec3(0.0f));
		shader.setVec3("dirLights[1].diffuse", glm::vec3(0.6f));
		shader.setVec3("dirLights[1].specular", glm::vec3(0.6f));

		shader.setVec3("dirLights[2].direction", 0.0f, -1.0f, 1.0f);
		shader.setVec3("dirLights[2].ambient", glm::vec3(0.0f));
		shader.setVec3("dirLights[2].diffuse", glm::vec3(0.6f));
		shader.setVec3("dirLights[2].specular", glm::vec3(0.6f));

		shader.setInt("numPointLights", 0);
		shader.setInt("numSpotLights", 0);

		// view/projection transformations
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		float aspect = float(viewport[2]) / float(viewport[3]);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, ZNEAR, ZFAR);
		shader.setMat4("projection", projection);
		shader.setMat4("view", camera.GetViewMatrix());

		// render the loaded model
		glm::mat4 modelMat = glm::mat4(1.0f);
		//modelMat = glm::rotate(modelMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::translate(modelMat, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", modelMat);

		model1.Draw(shader);

		//modelMat = glm::mat4(1.0f);
		////modelMat = glm::rotate(modelMat, -glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//modelMat = glm::translate(modelMat, glm::vec3(-1.0f, -0.5f, 0.8f)); // translate it down so it's at the center of the scene
		//modelMat = glm::scale(modelMat, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		//shader.setMat4("model", modelMat);

		//model2.Draw(shader);

		//modelMat = glm::mat4(1.0f);
		////modelMat = glm::rotate(modelMat, -glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//modelMat = glm::translate(modelMat, glm::vec3(1.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
		//modelMat = glm::scale(modelMat, glm::vec3(0.14f));	// it's a bit too big for our scene, so scale it down
		//shader.setMat4("model", modelMat);

		//model3.Draw(shader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	float speedMul = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		speedMul = 0.1f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		speedMul = 5.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime, speedMul);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime, speedMul);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime, speedMul);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime, speedMul);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime, speedMul);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime, speedMul);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(float(xoffset), float(yoffset));
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(float(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	if (width > 0 && height > 0)
		glViewport(0, 0, width, height);
}
