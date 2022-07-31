#include "shaderprogram.h"

ShaderProgram::ShaderProgram() {};

ShaderProgram::~ShaderProgram() {};

void ShaderProgram::create(std::string& vs, std::string& fs, LogicalDevice& logicaldevice) {
	vertexShaderFile = vs;
	fragmentShaderFile = fs;

	// generate shader bytecode
	std::vector<char> vertShaderCode = readShaderCode(vs);
	std::vector<char> fragShaderCode = readShaderCode(fs);

	// create shader modules for our vert and frag shader bytecode
	vertexShaderModule = createShaderModule(vertShaderCode, logicaldevice);
	fragmentShaderModule = createShaderModule(fragShaderCode, logicaldevice);

	// create info struct for vert shader stage
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShaderModule;
	shaderStages[0].pName = "main";
	shaderStages[0].pSpecializationInfo = nullptr;

	// create info struct for frag shader stage
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragmentShaderModule;
	shaderStages[1].pName = "main";
	shaderStages[1].pSpecializationInfo = nullptr;

	// get binding/attrib description for our vertices
	bindingDescription = Vertex::getBindingDescription();
	attributeDescriptions = Vertex::getAttributeDescriptions();
}

// read file as binary data and fill vector with bytes
std::vector<char> ShaderProgram::readShaderCode(const std::string& filename) {
	// start at end to get size
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

// creates a shader module out of a shader in a bytecode array
VkShaderModule ShaderProgram::createShaderModule(const std::vector<char>& code, LogicalDevice& logicaldevice) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(logicaldevice.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void ShaderProgram::destroyShaderModules(LogicalDevice& logicaldevice) {
	// destroy our shader modules
	vkDestroyShaderModule(logicaldevice.device, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(logicaldevice.device, vertexShaderModule, nullptr);
}

void ShaderProgram::destroy(LogicalDevice& logicaldevice) {
	destroyShaderModules(logicaldevice);
}