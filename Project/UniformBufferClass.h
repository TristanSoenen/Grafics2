#pragma once
#include "Structs.h"

class UniformBufferClass
{
public:
	UniformBufferClass() = default;
	~UniformBufferClass() = default;

	void createUniformBuffers(const VkDevice& device, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*> uniformBuffersMapped);
	void updateUniformBuffer(const VkDevice& device, uint32_t currentImage, VkExtent2D swapChainExtent, std::vector<void*>& uniformBuffersMapped);
private:

};
