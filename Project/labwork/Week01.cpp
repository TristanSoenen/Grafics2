#include "vulkanbase/VulkanBase.h"

void VulkanBase::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Milestone1: Tristan Soenen 2DAE19", nullptr, nullptr);
	camera = new Camera(window, glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
	glfwSetWindowUserPointer(window, this);
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) 
	{
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->mouseEvent(window, button, action, mods);
	});


}

void VulkanBase::drawScene() 
{
	//triangle mesh here
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}