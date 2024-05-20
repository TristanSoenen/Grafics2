#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZER

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"
#include "Structs.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <MachineShader.h>
#include "Buffer.h"
#include "Descriptor.h"
#include "UniformBufferClass.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>
#include <unordered_map>
#include "Camera.h"


const std::string MODEL_PATH = "resources/vehicle.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	//std::vector<Vertex> vertices = {
	//{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	//{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	//{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	//{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	//};

	//std::vector<uint16_t> indices = {
	//0, 1, 2, 2, 3, 0
	//};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	////Cube
	//std::vector<Vertex> vertices = 
	//{
	//	{{-1, 1, -4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   // Vertex 0
	//	{{1, 1, -4.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},    // Vertex 1
	//	{{1, -1, -4.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Vertex 2
	//	{{-1, -1, -4.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},  // Vertex 3

	//	 //Z plane
	//	{{-1.0f, 1.0f, -5.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},   // Vertex 4
	//	{{1.0f, 1.0f, -5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},    // Vertex 5
	//	{{1.0f, -1.0f, -5.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Vertex 6
	//	{{-1.0f, -1.0f, -5.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},   // Vertex 7
	//};

	//std::vector<uint32_t> indices = 
	//{
	//	//XY plane
	//	0, 1, 2, // First triangle
	//	2, 3, 0, // Second triangle

	//	//Z plane
	//	4, 5, 6, // First triangle
	//	6, 7, 4, // Second triangle

	//	//Connecting vertices between planes
	//	0, 1, 5, // First triangle
	//	5, 4, 0, // Second triangle

	//	1, 2, 6, // First triangle
	//	6, 5, 1, // Second triangle

	//	2, 3, 7, // First triangle
	//	7, 6, 2, // Second triangle

	//	3, 0, 4, // First triangle
	//	4, 7, 3,  // Second triangle
	//};


	//Mesh mesh1{ vertices, indices };


	//std::vector<Vertex> vertices1 = {
	//{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	//{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	//{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	//{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	//{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
	//};

	//std::vector<uint32_t> indices1 = {
	//	0, 1, 2, 2, 3, 0,
	//	4, 5, 6, 6, 7, 4
	//};
	//Mesh mesh2{ vertices1, indices1 };

	//std::vector<Mesh> meshes = { mesh1 };


	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped = {nullptr};

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	uint32_t currentFrame = 0;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	Camera* camera;
	float lastFrameTime = 0.0f;
	bool mousePressed = false;

	

	void initVulkan() 
	{

		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();
		
		// week 03
		m_MachineShader.initialize(device);
		createRenderPass();
		m_Descriptorclass.CreateDescriptorSetLayout(device, descriptorSetLayout);
		createGraphicsPipeline();
		// week 02
		createCommandPool();
		m_Bufferclass.Initialize(physicalDevice, commandPool, graphicsQueue);
		createColorResources();
		createDepthResources();
		createFrameBuffers();

		LoadModel();

		m_Bufferclass.createVertexBuffer(device, vertices, vertexBuffer, vertexBufferMemory);
		m_Bufferclass.createIndexBuffer(device, indices, indexBuffer, indexBufferMemory);
		

		createUniformBuffers();
		//m_UniBufferClass.createUniformBuffers(device, uniformBuffers, uniformBuffersMemory, uniformBuffersMapped);
		m_Descriptorclass.createDescriptorPool(device, descriptorPool);
		m_Descriptorclass.createDescriptorSets(device, uniformBuffers, descriptorPool, descriptorSets, descriptorSetLayout);
		createCommandBuffer();

		// week 06
		createSyncObjects();

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			//camera
			float currentFrameTime = glfwGetTime();
			float deltaTime = currentFrameTime - lastFrameTime;
			lastFrameTime = currentFrameTime;
			camera->processKeyboard(deltaTime);
			camera->processMouseMovement(window);
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() 
	{
		delete camera;
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(device, uniformBuffers[i], nullptr);
			vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
		}

		vkDestroyImageView(device, colorImageView, nullptr);
		vkDestroyImage(device, colorImage, nullptr);
		vkFreeMemory(device, colorImageMemory, nullptr);

		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);

		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);

		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		vkDestroyCommandPool(device, commandPool, nullptr);
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// camera 
	void mouseEvent(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (action == GLFW_PRESS)
			{
				// Get initial mouse position
				std::cout << " RightClicked\n";
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				camera->SetMouseDown(true);
			}
			else if(action == GLFW_RELEASE)
			{
				// Reset drag start when mouse button is released
				std::cout << " RightClicked released\n";
				camera->SetMouseDown(false);
			}
		}
		
	}
	//multisampeling
	VkSampleCountFlagBits getMaxUsableSampleCount() 
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	void createColorResources() 
	{
		VkFormat colorFormat = swapChainImageFormat;

		createImage(swapChainExtent.width, swapChainExtent.height, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
		colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);


	}

	//loading Model
	void LoadModel()
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) 
		{
			throw std::runtime_error(warn + err);
		}

		//std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				//if (uniqueVertices.count(vertex) == 0) 
				//{
				//	uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				//	vertices.push_back(vertex);
				//}


				vertices.push_back(vertex);

				indices.push_back(indices.size());
			}
		}
	}

	void createDepthResources()
	{
		VkFormat depthFormat = findDepthFormat();

		createImage(swapChainExtent.width, swapChainExtent.height, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = numSamples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_Bufferclass.findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
	{
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat findDepthFormat() 
	{
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool hasStencilComponent(VkFormat format) 
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void updateUniformBuffer(uint32_t currentImage)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.f));
		ubo.view = camera->getViewMatrix(); //glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = /*camera.GetProjectionMatrix()*/glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 20.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void createUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_Bufferclass.createBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	// important to initialize before creating the graphics pipeline
	MachineShader m_MachineShader{ "shaders/shader.vert.spv","shaders/shader.frag.spv"};
	Buffer m_Bufferclass;
	Descriptor m_Descriptorclass;
	UniformBufferClass m_UniBufferClass;

	void initWindow();
	void drawScene();

	// Week 02
	// Queue families
	// CommandBuffer concept

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;


	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void drawFrame(uint32_t imageIndex);
	void createCommandBuffer();
	void createCommandPool(); 
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;

	void createFrameBuffers();
	void createRenderPass();
	void createGraphicsPipeline();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};