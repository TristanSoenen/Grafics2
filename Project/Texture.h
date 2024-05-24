#pragma once
#include "vulkanbase/VulkanBase.h"

class Texture
{
	Texture() = default;
	~Texture() = default;
	void Initialize(const VkPhysicalDevice& physicalDevice, Buffer bufferClass);
	void createTextureImage(const VkDevice& device, const std::string texture, VkImage& textureimage, VkDeviceMemory& textureimagememory);
	void createImage(const VkDevice& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(const VkDevice& device, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(const VkDevice& device, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createTextureImageView(const VkDevice& device, VkImageView& textureimageview, VkImage& textureimage);
	VkImageView Texture::createImageView(const VkDevice& device, VkImage image, VkFormat format);
	void createTextureSampler(const VkDevice& device, const VkPhysicalDevice& physicalDevice, VkSampler& texturesampler);

private: 
	VkPhysicalDevice m_PhysicalDevice;
	Buffer m_Buffer;
};
