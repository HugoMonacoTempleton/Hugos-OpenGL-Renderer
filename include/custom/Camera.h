#ifndef CAMERA_H
#define CAMERA_H

#include <custom/Time.h>

#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/* CAMERA: Represents a camera in scene as well as containing information about screen properties and space transformation matrices. */
/*-----------------------------------------------------------------------------------------------------------------------------------*/
class Camera {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw = -90.0f;	
	float pitch = 0.0f;
	int scrWidth, scrHeight;
	unsigned int uboMatrices;
	unsigned int uboCameraScreen;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	bool firstMouse = true;
	bool sceneMovement = false;

public:
	Camera(int Width, int Height){
		scrWidth = Width;
		scrHeight = Height;		
	}

	void InitializeBuffers() {
		// Generate space transformation matrix UBO
		glGenBuffers(1, &uboMatrices);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

		// Generate camera and screen properties UBO
		glGenBuffers(1, &uboCameraScreen);
		glBindBuffer(GL_UNIFORM_BUFFER, uboCameraScreen);
		glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboCameraScreen, 0, 3 * sizeof(glm::vec4));
	}

	void Update() {
		// Set camera and screen UBO data
		glBindBuffer(GL_UNIFORM_BUFFER, uboCameraScreen);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(cameraPos));
		glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(cameraFront));
		glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(1.0 / scrWidth, 1.0 / scrHeight, scrWidth, scrHeight)));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Set space transformation matrix UBO data
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	/* Resize window */
	void frameBufferSizeChange(int Width, int Height) {
		scrWidth = Width;
		scrHeight = Height;
		glViewport(0, 0, scrWidth, scrHeight);
		
	}

	/* Keyboard input */
	void ProcessKeyboardInput(GLFWwindow* window) {
		// Close program
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		// Movement
		float cameraSpeed = 2.5f * Time::DeltaTime();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraSpeed * cameraUp;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraUp;
	}

	/* Mouse position input */
	void ProcessMouseInput(double xPos, double yPos) {
		// Check for middle mouse button
		if (!sceneMovement) {
			firstMouse = true;
			return;
		}
		
		if (firstMouse) {
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}

		float x = static_cast<float>(xPos);
		float y = static_cast<float>(yPos);

		float xoffset = x - lastX;
		float yoffset = lastY - y; // reversed since y-coordinates range from bottom to top
		lastX = xPos;
		lastY = yPos;

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		yaw += xoffset;
		pitch += yoffset;


		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	/* Returns a model matrix for a billboarded texture */
	glm::mat4 BillboardMatrix(glm::vec3 position) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		float crossX = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(cameraFront.x, 0.0f, cameraFront.z)).y > 0.0f ? 1.0f : -1.0f;
		model = glm::rotate(model, crossX * glm::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)))), glm::vec3(0.0f, 1.0f, 0.0f));
		float crossY = glm::cross(glm::vec3(cameraFront.x, cameraFront.y, 0.0f), glm::vec3(cameraFront.x, 0.0f, 0.0f)).z * cameraFront.x > 0.0f ? 1.0f : -1.0f;
		model = glm::rotate(model, crossY * glm::acos(glm::dot(glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)), cameraFront)), glm::vec3(1.0f, 0.0f, 0.0f));
		return model;
	}

	/* Mouse middle button input */
	void ProcessMouseButtonInput(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
			sceneMovement = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
			sceneMovement = false;
		}
	}

	int GetWidth() {
		return scrWidth;
	}

	int GetHeight() {
		return scrHeight;
	}

	void Free() {
		glDeleteBuffers(1, &uboCameraScreen);
		glDeleteBuffers(1, &uboMatrices);
	}
};
#endif
