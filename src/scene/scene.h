#ifndef SCENE_H
#define SCENE_H


#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "../render/renderpass.h"
#include "../image/msaabuffer.h"
#include "../device/logicaldevice.h"
#include "camera/camera.h"

#include "primitive.h"

using json = nlohmann::json;

class Scene {
public:
	Scene();
	~Scene();

	void createMaterialFromShaderFiles(std::string& vs, std::string& fs, LogicalDevice& logicaldevice, Swapchain& swapchain, DescriptorSetLayout& descriptorsetlayout, RenderPass& renderpass, MSAABuffer& msaabuffer);

	void loadSceneFromJSON(std::string& filePath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, CommandPool& commandpool, DescriptorSetLayout& descriptorsetlayout, DescriptorPool& descriptorpool, RenderPass& renderpass, MSAABuffer& msaabuffer, uint32_t maxFramesinFlight);

	// scene data
	std::vector<std::unique_ptr<Primitive>>			m_primitives;
	std::vector<std::unique_ptr<Mesh>>				m_meshes;
	std::vector<Light*>								m_lights;
	Camera											m_camera;

	std::vector<std::unique_ptr<Material>>			m_materials;

private:

};

void recursive_iterate(const json& j);

#endif
