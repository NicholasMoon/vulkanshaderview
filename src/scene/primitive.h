#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "../render/renderpass.h"
#include "../image/msaabuffer.h"
#include "../device/logicaldevice.h"

#include "geometry/mesh.h"
#include "light/light.h"
#include "light/pointlight.h"
#include "light/directionallight.h"
#include "light/spotlight.h"
#include "../ubo.h"

class RenderPass;
class MSAABuffer;


class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(std::string& vs, std::string& fs);
	~ShaderProgram();

	void create(LogicalDevice& logicaldevice);
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

class GraphicsPipeline {
public:
	GraphicsPipeline();
	~GraphicsPipeline();

	void createGraphicsPipeline(ShaderProgram& shaderprogram, Swapchain& swapchain, LogicalDevice& logicaldevice, DescriptorSetLayout& descriptorSetLayout, RenderPass& renderpass, MSAABuffer& msaabuffer);

	void destroyGraphicsPipeline(LogicalDevice& logicaldevice);

	VkPipeline vkGraphicsPipeline; // handle for entire graphics pipeline
	VkPipelineLayout vkPipelineLayout; // handle for uniform shader variables in pipeline

	// properties of the pipeline we wish to make dynamic (changed without recreating pipeline)
	std::vector<VkDynamicState> dynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_LINE_WIDTH
	};

private:

};

class Material {
public:
	Material();
	~Material();



	ShaderProgram		m_shader;
	GraphicsPipeline	m_pipeline;

private:

};



class Primitive {
public:
	Primitive();
	~Primitive();

	// creates vertex buffer to store vert data
	void createVertexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);

	// creates index buffer to store vert index data
	void createIndexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool);

	// create buffers for the uniform variables (one for each frame in flight)
	void createUniformBuffers(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice);


	void toString();

	
	void setRotation(glm::vec3 r);
	void setTranslation(glm::vec3 t);
	void setScale(glm::vec3 s);

	void rotate(glm::vec3 r);
	void translate(glm::vec3 t);
	void scale(glm::vec3 s);

	void recalculateModelMatrix();

	// primitive attributes
	Mesh*										m_geometry; // for now, don't support instancing/reusing these fields...
	std::unique_ptr<Light>						m_light;
	Material*									m_material;

	// transform data
	glm::vec3									m_translation;
	glm::vec3									m_rotation;
	glm::vec3									m_scale;
	glm::mat4									m_modelMatrix;
	glm::mat4									m_modelMatrixInvTrans;

	UniformBufferObject							m_ubo;

	// vulkan buffer data
	Texture										m_texture;    // allocated on a per-primitive basis
	Texture										m_normalmap;
	DataBuffer									m_vertexbuffer;
	DataBuffer									m_indexbuffer;
	std::vector<DataBuffer>						m_uniformbuffers;
	DescriptorSets								m_descriptorsets;

private:

};


#endif
