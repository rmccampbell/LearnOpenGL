#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::quat Orientation;
	// Camera options
	float MovementSpeed = SPEED;
	float MouseSensitivity = SENSITIVITY;
	float Zoom = ZOOM;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
	{
		Position = position;
		front = glm::normalize(front);
		glm::vec3 right = glm::normalize(glm::cross(front, up));
		up = glm::normalize(glm::cross(right, front));
		Orientation = glm::quat(glm::mat3(right, up, -front));
	}

	// Constructor with scalar values
	Camera(float posX, float posY, float posZ,
		float frontX, float frontY, float frontZ,
		float upX, float upY, float upZ)
		: Camera({ posX, posY, posZ }, { frontX, frontY, frontZ }, { upX, upY, upZ })
	{}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::translate(glm::mat4_cast(glm::conjugate(Orientation)), -Position);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(CameraMovement direction, float deltaTime, float speedMul)
	{
		float velocity = MovementSpeed * deltaTime * speedMul;
		glm::vec3 dir;
		switch (direction) {
		case FORWARD:
			dir = glm::vec3( 0, 0,-1); break;
		case BACKWARD:
			dir = glm::vec3( 0, 0, 1); break;
		case LEFT:
			dir = glm::vec3(-1, 0, 0); break;
		case RIGHT:
			dir = glm::vec3( 1, 0, 0); break;
		case UP:
			dir = glm::vec3( 0, 1, 0); break;
		case DOWN:
			dir = glm::vec3( 0,-1, 0); break;
		}
		Position += Orientation * dir * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		//float yaw = -glm::radians(xoffset * MouseSensitivity);
		//float pitch = glm::radians(yoffset * MouseSensitivity);
		//Orientation = glm::rotate(Orientation, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
		//Orientation = glm::rotate(Orientation, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 axis(yoffset, -xoffset, 0.0f);
		float length = glm::length(axis);
		if (length) {
			float angle = glm::radians(length * MouseSensitivity);
			Orientation = glm::rotate(Orientation, angle, axis / length);
			Orientation = glm::normalize(Orientation);
		}
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom = glm::clamp(Zoom - yoffset, 1.0f, 45.0f);
	}
};
