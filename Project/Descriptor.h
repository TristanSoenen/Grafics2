#pragma once
#include "Structs.h"

class Descriptor
{
public:
	Descriptor();
	~Descriptor() = default;
	void CreateDescriptorSetLayout(const VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
	void createDescriptorSets(const VkDevice& device, std::vector<VkBuffer>& uniformBuffers, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkImageView>& imageViewVector, std::vector<VkSampler>& samplerVector);
	void createDescriptorPool(const VkDevice& device, VkDescriptorPool& descriptorPool);
private:

};

