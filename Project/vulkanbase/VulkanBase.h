#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZER

#include <vector>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"
#include "Structs.h"
#include "tiny_obj_loader.h"
#include "stb_image.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <stdexcept>

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
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <chrono>
#include <unordered_map>
#include "Camera.h"
#include <fstream>
const int MAX_FRAMES_IN_FLIGHT = 5; //This is important for the amount of descriptors you want to have the buffer + amount of textures

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
	
	std::vector<Mesh> meshes;
	VkDescriptorSetLayout descriptorSetLayout;

	uint32_t currentFrame = 0;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	glm::vec2 dragstart{};
	glm::vec2 rotation{};
	std::vector<objectInfo> sceneObjectsInfo;

	dae::Camera camera{ window, glm::vec3{0.0f, 0.0f, 0.0f}, 90.0f};
	float lastFrameTime = 0.0f;
	int m_CurrentIndex = 3;
	bool m_KeyPress = false;

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

		ReadJSON(sceneObjectsInfo);
		meshes.resize(sceneObjectsInfo.size());
		int positionIndex = 0;
		for (auto& mesh : meshes)
		{
			//start splitting for meshes.
			LoadModel(mesh, sceneObjectsInfo[positionIndex].position, sceneObjectsInfo[positionIndex].scale, sceneObjectsInfo[positionIndex].rotationAngle, sceneObjectsInfo[positionIndex].path);
			mesh.VkImageVector.resize(4);
			mesh.VkTextureMemoryVector.resize(4);
			mesh.VkImageViewVector.resize(4);
			mesh.VkSamplerVector.resize(4);

			//Create Textures
			for (int i = 0; i < sceneObjectsInfo[positionIndex].maps.size(); i++)
			{
				createTextureImage(sceneObjectsInfo[positionIndex].maps[i], mesh.VkImageVector[i], mesh.VkTextureMemoryVector[i]);
				createTextureImageView(mesh.VkImageViewVector[i], mesh.VkImageVector[i]);
				createTextureSampler(mesh.VkSamplerVector[i]);
			}

			m_Bufferclass.createVertexBuffer(device, mesh.vertices, mesh.vertexBuffer, mesh.vertexBufferMemory);
			m_Bufferclass.createIndexBuffer(device, mesh.indices, mesh.indexBuffer, mesh.indexBufferMemory);

			createUniformBuffers(mesh);
			m_Descriptorclass.createDescriptorPool(device, mesh.descriptorPool);
			m_Descriptorclass.createDescriptorSets(device, mesh.uniformBuffers, mesh.descriptorPool, mesh.descriptorSets, descriptorSetLayout, mesh.VkImageViewVector, mesh.VkSamplerVector);
			positionIndex++;
		}
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
			camera.Update(deltaTime, window);
			ChangeState();
			// week 06
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() 
	{
		for (auto& mesh : meshes)
		{
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				vkDestroyBuffer(device, mesh.uniformBuffers[i], nullptr);
				vkFreeMemory(device, mesh.uniformBuffersMemory[i], nullptr);
			}
		}

		vkDestroyImageView(device, colorImageView, nullptr);
		vkDestroyImage(device, colorImage, nullptr);
		vkFreeMemory(device, colorImageMemory, nullptr);

		for (auto& mesh : meshes)
		{
			vkDestroyDescriptorPool(device, mesh.descriptorPool, nullptr);
		}
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		for (auto& mesh : meshes)
		{
			for (int i = 0; i < mesh.VkSamplerVector.size(); i++)
			{
				vkDestroySampler(device, mesh.VkSamplerVector[i], nullptr);
			}
			for (int i = 0; i < mesh.VkImageViewVector.size(); i++)
			{
				vkDestroyImageView(device, mesh.VkImageViewVector[i], nullptr);
			}

			for (int i = 0; i < mesh.VkImageVector.size(); i++)
			{
				vkDestroyImage(device, mesh.VkImageVector[i], nullptr);
			}
			for (int i = 0; i < mesh.VkTextureMemoryVector.size(); i++)
			{
				vkFreeMemory(device, mesh.VkTextureMemoryVector[i], nullptr);
			}
			vkDestroyBuffer(device, mesh.indexBuffer, nullptr);
			vkFreeMemory(device, mesh.indexBufferMemory, nullptr);
			vkDestroyBuffer(device, mesh.vertexBuffer, nullptr);
			vkFreeMemory(device, mesh.vertexBufferMemory, nullptr);
		}


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

	//CameraMouseMOVe
	void VulkanBase::mouseMove(GLFWwindow* window, double xpos, double ypos)
	{
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		if (state == GLFW_PRESS)
		{
			float dx = static_cast<float>(xpos) - dragstart.x;
			if (dx > 0)
			{
				camera.UpdateYawPlus();
			}
			else
			{
				camera.UpdateYawMin();
			}
		}

		int otherstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (otherstate == GLFW_PRESS)
		{
			float dx = static_cast<float>(ypos) - dragstart.y;
			if (dx > 0)
			{
				camera.UpdatePitchPlus();
			}
			else
			{
				camera.UpdatePitchMin();
			}
		}
	}

	void VulkanBase::mouseEvent(GLFWwindow* window, int button, int action, int mods)
	{
		double xpos, ypos;
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &xpos, &ypos);
			dragstart.x = static_cast<float>(xpos);
			//dragstart.y = static_cast<float>(ypos);
		}
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &xpos, &ypos);
			//dragstart.x = static_cast<float>(xpos);
			dragstart.y = static_cast<float>(ypos);
		}
	}

	void VulkanBase::ChangeState()
	{
		int state = glfwGetKey(window, GLFW_KEY_F6);
		
		if (state == GLFW_PRESS && m_KeyPress == false)
		{
			m_KeyPress = true;
			++m_CurrentIndex;
			if (m_CurrentIndex > 3)
			{
				m_CurrentIndex = 0;
			}
		}
		if(state == GLFW_RELEASE && m_KeyPress == true)
		{
			m_KeyPress = false;
		}
	}

	void ReadJSON(std::vector<objectInfo>& objectinfoVector)
	{
		//chat gpt helped with this
		std::ifstream file("resources/scene.json");
		if (!file.is_open()) {
			std::cerr << "Failed to open file\n";
		}
		// Parse JSON data
		nlohmann::json scene_data;
		file >> scene_data;
		file.close();
		// Access your scene data and store in variables
		for (const auto& obj : scene_data["objects"]) 
		{
			objectInfo object;
			object.path = obj["path"];
			object.position = glm::vec3{ obj["position"][0], obj["position"][1], obj["position"][2] };
			object.scale = glm::vec3{ obj["scale"][0], obj["scale"][1], obj["scale"][2] };
			object.rotationAngle = obj["rotationAngle"];
			object.maps = obj["maps"];
			objectinfoVector.push_back(object);
		}
	}

	// Texture
	void createTextureSampler(VkSampler& texturesampler)
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1.0f;//properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &texturesampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	void createTextureImageView(VkImageView& textureimageview , VkImage& textureimage)
	{
		textureimageview = createImageView(textureimage, VK_FORMAT_R8G8B8A8_SRGB);
	}

	VkImageView createImageView(VkImage image, VkFormat format) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
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

	void createTextureImage(const std::string texture, VkImage& textureimage, VkDeviceMemory& textureimagememory)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(texture.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		m_Bufferclass.createBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, stagingBufferMemory);

		stbi_image_free(pixels);

		createImage(texWidth, texHeight, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureimage, textureimagememory);

		transitionImageLayout(textureimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, textureimage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
		transitionImageLayout(textureimage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = m_Bufferclass.beginSingleTimeCommands(device);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else 
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_Bufferclass.endSingleTimeCommands(device, commandBuffer);
	}

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) 
	{
		VkCommandBuffer commandBuffer = m_Bufferclass.beginSingleTimeCommands(device);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage
		(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
		m_Bufferclass.endSingleTimeCommands(device, commandBuffer);
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
	void LoadModel(Mesh& mesh, glm::vec3 position, glm::vec3 scale, float angle, std::string& path)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) 
		{
			throw std::runtime_error(warn + err);
		}


		//chat gpt helpend with the trs
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position); // Translation
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1.0f, 0)); // Rotation
		model = glm::scale(model, scale); // Scaling
		//std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				glm::vec4 pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2],
				1.0f
				};

				pos = model * pos;
				vertex.pos = {
					pos.x, pos.y, pos.z
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				/*if (uniqueVertices.count(vertex) == 0) 
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}*/


				mesh.vertices.push_back(vertex);

				mesh.indices.push_back(mesh.indices.size());
			}

			for (uint32_t i = 0; i < mesh.indices.size(); i += 3)
			{
				uint32_t index0 = mesh.indices[i];
				uint32_t index1 = mesh.indices[size_t(i) + 1];
				uint32_t index2 = mesh.indices[size_t(i) + 2];
				const glm::vec3& p0 = mesh.vertices[index0].pos;
				const glm::vec3& p1 = mesh.vertices[index1].pos;
				const glm::vec3& p2 = mesh.vertices[index2].pos;
				const glm::vec2 uv0 = mesh.vertices[index0].texCoord;
				const glm::vec2 uv1 = mesh.vertices[index1].texCoord;
				const glm::vec2 uv2 = mesh.vertices[index2].texCoord;
				const glm::vec3 edge0 = p1 - p0;
				const glm::vec3 edge1 = p2 - p0;
				const glm::vec2 diffX = glm::vec2(uv1.x - uv0.x, uv2.x - uv0.x);
				const glm::vec2 diffY = glm::vec2(uv1.y - uv0.y, uv2.y - uv0.y);
				float r = 1.0f / (diffX.x * diffY.y - diffX.y - diffY.x);

				glm::vec3 tangent = (edge0 * diffY.y - edge1 * diffX.x) * r;
				mesh.vertices[index0].tangent += tangent;
				mesh.vertices[index1].tangent += tangent;
				mesh.vertices[index2].tangent += tangent;
			}

			for (auto& v : mesh.vertices)
			{
				glm::vec3 projection = (glm::dot(v.tangent, v.normal) / glm::dot(v.normal, v.normal)) * v.normal;
				v.tangent = v.tangent - projection;
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
		for (auto& mesh : meshes)
		{
			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			UniformBufferObject ubo{};
			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.f));
			ubo.view = camera.GetViewMatrix();
			ubo.proj = camera.GetProjectionMatrix();
			ubo.invView = camera.invViewMatrix;
			ubo.mapindex = m_CurrentIndex;
			ubo.proj[1][1] *= -1;


			memcpy(mesh.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
		}


	}

	void createUniformBuffers(Mesh& mesh)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		mesh.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		mesh.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
		mesh.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
		

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			m_Bufferclass.createBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mesh.uniformBuffers[i], mesh.uniformBuffersMemory[i]);

			vkMapMemory(device, mesh.uniformBuffersMemory[i], 0, bufferSize, 0, &mesh.uniformBuffersMapped[i]);
		}
	}

	void createSurface() 
	{
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