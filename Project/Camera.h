#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace dae
{
	struct Camera
	{
		Camera(GLFWwindow* window, const glm::vec3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{

		}
		glm::vec3 origin{};
		float fovAngle{ 90.f };
		float fov{ tanf(glm::radians(fovAngle) / 2.f) };
		const float aspect_Ratio{ 1.33333f };

		float nearPlane{ 0.1f };
		float farPlane{ 100.0f };

		const float cameraSpeed{ 20.0f };
		const float cameraSpeedMouse{ 10.0f };

		glm::vec3 forward{ 0, 0, 1 };
		glm::vec3 up{ 0, 1, 0 };
		glm::vec3 right{ 1, 0, 0 };

		float totalPitch{};
		float totalYaw{};

		glm::mat4 invViewMatrix{};
		glm::mat4 viewMatrix{};
		glm::mat4 projectionMatrix{};


		void Initialize(float _fovAngle = 90.f, glm::vec3 _origin = { 0.f,0.f,0.f })
		{
			fovAngle = _fovAngle;
			fov = tanf(glm::radians(_fovAngle) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			glm::vec3 front;
			front.x = cos(glm::radians(totalYaw)) * cos(glm::radians(totalPitch));
			front.y = sin(glm::radians(totalPitch));
			front.z = sin(glm::radians(totalYaw)) * cos(glm::radians(totalPitch));
			forward = glm::normalize(front);

			right = glm::normalize(glm::cross(forward, up));
			up = glm::normalize(glm::cross(right, forward));

			viewMatrix = glm::lookAt(origin, origin + forward, up);
		}

		void CalculateProjectionMatrix()
		{
			projectionMatrix = glm::perspective(glm::radians(fovAngle), aspect_Ratio, nearPlane, farPlane);
		}

		glm::mat4 GetViewMatrix() const { return viewMatrix; }
		glm::mat4 GetProjectionMatrix() const { return projectionMatrix; }

		void Update(float deltaTime, GLFWwindow* window)
		{

			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				origin += deltaTime * cameraSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				origin -= deltaTime * cameraSpeed * forward;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				origin -= deltaTime * cameraSpeed * right;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				origin += deltaTime * cameraSpeed * right;
			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}

		void UpdateYawPlus()
		{
			++totalYaw;
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}

		void UpdateYawMin()
		{
			--totalYaw;
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}

		void UpdatePitchPlus()
		{
			++totalPitch;
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}

		void UpdatePitchMin()
		{
			--totalPitch;
			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}
	};
}