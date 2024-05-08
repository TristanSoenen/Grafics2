//#pragma once
//#include "vulkanbase/VulkanBase.h"
//#include "vulkan/vulkan_core.h"
//#include "vulkanbase/VulkanUtil.h"
//#include "glm/glm.hpp"
//#include <array>
//#include <vector>
//
//struct Vertex
//{
//	glm::vec2 pos;
//	glm::vec3 color;
//
//	static VkVertexInputBindingDescription getBindingDescription()
//	{
//		VkVertexInputBindingDescription bindingDescription{};
//		bindingDescription.binding = 0;
//		bindingDescription.stride = sizeof(Vertex);
//		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//		return bindingDescription;
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
//	{
//		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
//		attributeDescriptions[0].binding = 0;
//		attributeDescriptions[0].location = 0;
//		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//		attributeDescriptions[0].offset = offsetof(Vertex, pos);
//
//		attributeDescriptions[1].binding = 0;
//		attributeDescriptions[1].location = 1;
//		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attributeDescriptions[1].offset = offsetof(Vertex, color);
//		return attributeDescriptions;
//	}
//};
//
//class Mesh
//{
//	Mesh(const VkDevice& device);
//
//	Mesh(const Mesh&) = delete;
//	Mesh(Mesh&&) = delete;
//	Mesh& operator=(const Mesh&) = delete;
//	Mesh& operator=(Mesh&&) = delete;
//
//	~Mesh() = default;
//
//	void CreateVertexBuffer()
//	{
//		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, vertices.data(), (size_t)bufferSize);
//		vkUnmapMemory(m_Device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
//
//		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
//
//		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
//		vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
//	}
//
//	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
//	{
//		VkBufferCreateInfo bufferInfo{};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = size;
//		bufferInfo.usage = usage;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create buffer!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
//	}
//
//	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
//	{
//		VkPhysicalDeviceMemoryProperties memProperties;
//		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//				return i;
//			}
//		}
//		throw std::runtime_error("failed to find suitable memory type!");
//	}
//
//	void CreateIndexBuffer()
//	{
//		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, indices.data(), (size_t)bufferSize);
//		vkUnmapMemory(m_Device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
//
//		copyBuffer(stagingBuffer, indexBuffer, bufferSize);
//
//		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
//		vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
//	}
//
//	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
//	{
//		VkCommandBufferAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandPool = commandPool;
//		allocInfo.commandBufferCount = 1;
//
//		VkCommandBuffer commandBuffer;
//		vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);
//
//		VkCommandBufferBeginInfo beginInfo{};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//		VkBufferCopy copyRegion{};
//		//copyRegion.srcOffset = 0; // Optional
//		//copyRegion.dstOffset = 0; // Optional
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//		vkEndCommandBuffer(commandBuffer);
//
//		VkSubmitInfo submitInfo{};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(graphicsQueue);
//
//		vkFreeCommandBuffers(m_Device, commandPool, 1, &commandBuffer);
//	}
//
//	void Draw(VkCommandBuffer commandBuffer);
//
//	void DestroyMesh();
//
//private:
//
//	VkDevice m_Device;
//
//	const std::vector<Vertex> vertices = {
//	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//	};
//
//	const std::vector<uint16_t> indices = {
//	0, 1, 2, 2, 3, 0
//	};
//
//	// device properties
//	VkDevice m_Device;
//	VkPhysicalDevice m_PhysicalDevice;
//	VkQueue m_Queue;
//
//	// buffer properties
//	std::vector<Vertex> m_Vertices;
//	const std::vector<uint16_t> m_Indices;
//};
