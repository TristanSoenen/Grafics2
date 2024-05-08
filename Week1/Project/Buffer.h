#pragma once
#include "Structs.h"

class Buffer
{
public:
	Buffer() = default;
	~Buffer() = default;

	void Initialize(VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graficsQueue);
	void createVertexBuffer( const VkDevice& device, std::vector<Vertex>& vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory );
	void createIndexBuffer( const VkDevice& device, std::vector<uint32_t>& indices, VkBuffer& indexBuffer, VkDeviceMemory& IndexBufferMemory );
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(const VkDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(const VkDevice& device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
private:
	VkPhysicalDevice m_PhysicalDevice;
	VkCommandPool m_CommandPool;
	VkQueue m_GraphicsQueue;
};
