#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>

#include "../../device/logicaldevice.h"
#include "../../vertex.h"


class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void create(std::string& vs, std::string& fs, LogicalDevice& logicaldevice);

	// creates a shader module out of a shader in a bytecode array
	VkShaderModule createShaderModule(const std::vector<char>& code, LogicalDevice& logicaldevice);

	// read file as binary data and fill vector with bytes
	std::vector<char> readShaderCode(const std::string& filename);

	void destroyShaderModules(LogicalDevice& logicaldevice);

	void destroy(LogicalDevice& logicaldevice);




	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;

	VkPipelineShaderStageCreateInfo shaderStages[2];

	VkVertexInputBindingDescription bindingDescription;
	std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo;

private:
	std::string vertexShaderFile;
	std::string fragmentShaderFile;


};

#endif