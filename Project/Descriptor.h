#pragma once
#include "Structs.h"

class Descriptor
{
public:
	Descriptor();
	~Descriptor() = default;
	void CreateDescriptorSetLayout(const VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
	void createDescriptorSets(const VkDevice& device, std::vector<VkBuffer>& uniformBuffers, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout& descriptorSetLayout, VkImageView& textureImageView, VkSampler& textureSampler);
	void createDescriptorPool(const VkDevice& device, VkDescriptorPool& descriptorPool);
private:

};

