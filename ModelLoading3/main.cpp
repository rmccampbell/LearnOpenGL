#define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING
#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "lights.h"
#include "primitives.h"
#include "debug.h"

using namespace std;
namespace fs = std::filesystem;
using glm::vec3;

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
Camera camera(vec3(0.0f, 0.0f, 3.0f));
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastTime = 0.0f;

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
#ifndef NO_DISABLE_CURSOR
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// build and compile our shader program
	// ------------------------------------
	Shader shader("shaders/shader.vert", "shaders/shader.frag");

	// Load mesh
	// ---------

	Mesh model1 = makeSphere();
	//Mesh model1 = makeCube(true);
	Material mat;
	mat.ambient_color = vec3(1.0f);
	mat.diffuse_color = vec3(1.0f);
	mat.specular_color = vec3(0.5f);
	mat.shininess = 10.0f;
	mat.diffuse_texture = Texture("../Resources/textures/earth_sphere10k.jpg");
	//mat.diffuse_texture = Texture("../Resources/textures/cubenet.png");
	model1.material = &mat;

	Mesh lightMesh = makeSphere();
	Material lightMat;
	lightMat.diffuse_color = vec3(0.0f);
	lightMat.specular_color = vec3(0.0f);
	lightMat.ambient_color = vec3(0.0f);
	lightMat.emissive_color = vec3(1.0f);
	lightMesh.material = &lightMat;

	PointLight pointLights[] = {
		{
			.position = {1.0f, 1.0f, 1.0f},
			.constant = 1.0f,
			.linear = 0.09f,
			.quadratic = 0.032f,
			.ambient = vec3(0.2f),
			.diffuse = vec3(1.0f),
			.specular = vec3(1.0f),
		},
		{
			.position = {1.0f, 1.0f, 1.0f},
			.constant = 1.0f,
			.linear = 0.09f,
			.quadratic = 0.032f,
			.ambient = vec3(0.2f),
			.diffuse = vec3(1.0f),
			.specular = vec3(1.0f),
		},
	};

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentTime = float(glfwGetTime());
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("viewPos", camera.Position);

		//vec3 lightColor(1.0);
		vec3 lightColor = glm::clamp(glm::sin(currentTime * vec3(2.0f, 0.7f, 1.3f)), 0.0f, 1.0f) * 1.5f;
		pointLights[1].ambient = 0.2f * lightColor;
		pointLights[1].diffuse = pointLights[1].specular = lightColor;

		float r = 2.0f, t = currentTime * 0.5f;
		pointLights[0].position = { r * glm::cos(t), 0.0f, r * glm::sin(t) };

		shader.setInt("numDirLights", 0);
		shader.setInt("numPointLights", int(std::size(pointLights)));
		for (int i = 0; i < std::size(pointLights); i++) {
			pointLights[i].Apply(shader, "pointLights[" + to_string(i) + "]");
		}
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
		modelMat = glm::rotate(modelMat, currentTime * .2f, { 0.0f, 1.0f, 0.0f });
		//modelMat = glm::translate(modelMat, { 0.0f, -1.75f, 0.0f }); // translate it down so it's at the center of the scene
		//modelMat = glm::scale(modelMat, vec3(0.2f));	// it's a bit too big for our scene, so scale it down
		shader.setMat4("model", modelMat);

		model1.Draw(shader);

		for (PointLight& light : pointLights) {
			glm::mat4 modelMat = glm::mat4(1.0f);
			modelMat = glm::translate(modelMat, light.position);
			modelMat = glm::scale(modelMat, vec3(0.1f));
			shader.setMat4("model", modelMat);
			lightMat.emissive_color = light.diffuse;
			lightMesh.Draw(shader);
		}

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

#ifdef NO_DISABLE_CURSOR
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
#endif
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
