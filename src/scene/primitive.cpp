#include "primitive.h"

Primitive::Primitive() : m_geometry(), m_translation(glm::vec3(0)), m_rotation(glm::vec3(0)), m_scale(glm::vec3(1)), 
m_modelMatrix(glm::mat4(1.0)), m_modelMatrixInvTrans(glm::mat4(1.0)),
m_texture(), m_vertexbuffer(), m_indexbuffer(), m_descriptorsets() 
{};




void Primitive::toString() {

}

// creates vertex buffer to store vert data
void Primitive::createVertexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    m_vertexbuffer.vkBufferSize = sizeof(m_geometry->vertices[0]) * m_geometry->vertices.size();

    // create staging buffer
    DataBuffer stagingBuffer(m_vertexbuffer.vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, m_vertexbuffer.vkBufferSize, 0, &data);
    memcpy(data, m_geometry->vertices.data(), (size_t)m_vertexbuffer.vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    // create actual buffer
    m_vertexbuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    m_vertexbuffer.copyBuffer(stagingBuffer.vkBuffer, m_vertexbuffer.vkBuffer, m_vertexbuffer.vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// creates index buffer to store vert index data
void Primitive::createIndexBuffer(LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, CommandPool& commandpool) {
    m_indexbuffer.vkBufferSize = sizeof(m_geometry->indices[0]) * m_geometry->indices.size();

    DataBuffer stagingBuffer(m_indexbuffer.vkBufferSize);
    stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);

    void* data;
    vkMapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, 0, m_indexbuffer.vkBufferSize, 0, &data);
    memcpy(data, m_geometry->indices.data(), (size_t)m_indexbuffer.vkBufferSize);
    vkUnmapMemory(logicaldevice.device, stagingBuffer.vkBufferMemory);

    m_indexbuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicaldevice, physicaldevice);

    m_indexbuffer.copyBuffer(stagingBuffer.vkBuffer, m_indexbuffer.vkBuffer, m_indexbuffer.vkBufferSize, logicaldevice, commandpool);

    vkDestroyBuffer(logicaldevice.device, stagingBuffer.vkBuffer, nullptr);
    vkFreeMemory(logicaldevice.device, stagingBuffer.vkBufferMemory, nullptr);
}

// create buffers for the uniform variables (one for each frame in flight)
void Primitive::createUniformBuffers(uint32_t maxFramesinFlight, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice) {
    m_uniformbuffers.resize(maxFramesinFlight);

    for (size_t j = 0; j < maxFramesinFlight; ++j) {
        m_uniformbuffers[j].vkBufferSize = sizeof(UniformBufferObject);

        m_uniformbuffers[j].createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicaldevice, physicaldevice);
    }
}

void Primitive::setRotation(glm::vec3 r) {
    m_rotation = r;
    recalculateModelMatrix();
}

void Primitive::setTranslation(glm::vec3 t) {
    m_translation = t;
    recalculateModelMatrix();
}

void Primitive::setScale(glm::vec3 s) {
    m_scale = s;
    recalculateModelMatrix();
}

void Primitive::recalculateModelMatrix() {
    m_modelMatrix = glm::translate(glm::mat4(1.0f), m_translation)
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1, 0, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0, 1, 0))
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0, 0, 1))
        * glm::scale(glm::mat4(1.0f), m_scale);

    m_modelMatrixInvTrans = glm::inverse(glm::transpose(m_modelMatrix));
}

void Primitive::rotate(glm::vec3 r) {
    m_rotation += r;
    recalculateModelMatrix();
}

void Primitive::translate(glm::vec3 t) {
    m_translation += t;
    recalculateModelMatrix();
}

void Primitive::scale(glm::vec3 s) {
    m_scale += s;
    recalculateModelMatrix();
}


Primitive::~Primitive() {};



ShaderProgram::ShaderProgram() {};

ShaderProgram::ShaderProgram(std::string& vs, std::string& fs) {
    vertexShaderFile = vs;
    fragmentShaderFile = fs;
};

ShaderProgram::~ShaderProgram() {};

void ShaderProgram::create(std::string& vs, std::string& fs, LogicalDevice& logicaldevice) {
    vertexShaderFile = vs;
    fragmentShaderFile = fs;

    create(logicaldevice);
}

void ShaderProgram::create(LogicalDevice& logicaldevice) {
    // generate shader bytecode
    std::vector<char> vertShaderCode = readShaderCode(vertexShaderFile);
    std::vector<char> fragShaderCode = readShaderCode(fragmentShaderFile);

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






GraphicsPipeline::GraphicsPipeline() {}

GraphicsPipeline::~GraphicsPipeline() {}

void GraphicsPipeline::createGraphicsPipeline(ShaderProgram& shaderprogram, Swapchain& swapchain, LogicalDevice& logicaldevice, DescriptorSetLayout& descriptorsetlayout, RenderPass& renderpass, MSAABuffer& msaabuffer) {
    // create info struct for vertex input stage
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &shaderprogram.bindingDescription;; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(shaderprogram.attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = shaderprogram.attributeDescriptions.data(); // Optional

    // create info struct for primitive assembly stage
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // create viewport (rectangle of framebuffer we are writing to)
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain.vkSwapChainExtent.width;
    viewport.height = (float)swapchain.vkSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // create scissor rectangle (to crop image)
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.vkSwapChainExtent;

    // create info struct that combines viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // create rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // multisampling (AA) options
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = msaabuffer.msaaSamples; // for msaa
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // color blending options (single framebuffer)
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    // color blending options (global)
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    // specify which pipeline params to make dynamic (mutable without pipeline recreation)
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Uniform variables
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorsetlayout.vkDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(logicaldevice.device, &pipelineLayoutInfo, nullptr, &vkPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    // depth and stencil buffers
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    // struct containing all information regarding the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderprogram.shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = vkPipelineLayout;
    pipelineInfo.renderPass = renderpass.vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    // create graphics pipeline!
    if (vkCreateGraphicsPipelines(logicaldevice.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

// destroy graphics pipeline and pipeline layout (uniform variables)
void GraphicsPipeline::destroyGraphicsPipeline(LogicalDevice& logicaldevice) {
    // destroy shader modules
    vkDestroyPipelineLayout(logicaldevice.device, vkPipelineLayout, nullptr);
    vkDestroyPipeline(logicaldevice.device, vkGraphicsPipeline, nullptr);
}





Material::Material() {};

Material::~Material() {};