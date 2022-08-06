#include "scene.h"

Scene::Scene() {}

Scene::~Scene() {}

void Scene::loadSceneFromJSON(std::string& filePath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, CommandPool& commandpool, DescriptorSetLayout& descriptorsetlayout, DescriptorPool& descriptorpool, RenderPass& renderpass, MSAABuffer& msaabuffer, uint32_t maxFramesinFlight) {
	std::ifstream f(filePath);
	json j = json::parse(f);

	int primitive_count = 0;

    auto sceneData = j["scene"];
	for (auto s : sceneData.items()) {
		if (s.key() == "camera") {
			auto ref = s.value()["ref"];
			auto eye = s.value()["eye"];
			auto up = s.value()["worldUp"];
			auto fovy = s.value()["fov"];
			auto width = s.value()["width"];
			auto height = s.value()["height"];

			std::cout << "camera:" << std::endl;
			std::cout << "   ref: " << ref << std::endl;
			std::cout << "   eye: " << eye << std::endl;
			std::cout << "   up: " << up << std::endl;
			std::cout << "   fovy: " << fovy << std::endl;
			std::cout << "   width: " << width << std::endl;
			std::cout << "   height: " << height << std::endl;
		}
		else if (s.key() == "primitives") {
			for (auto p : s.value().items()) {
				auto name = p.value()["name"].get<std::string>();;
				auto obj_file = p.value()["obj_file"].get<std::string>();;
				auto material = p.value()["material"].get<std::string>();;
				auto transform = p.value()["transform"];
				auto translate = transform["translate"];
				auto rotate = transform["rotate"];
				auto scale = transform["scale"];
				auto vs = p.value()["vs"].get<std::string>();
				auto fs = p.value()["fs"].get<std::string>();
				auto albedo_map = p.value()["albedo_map"].get<std::string>();
				auto normal_map = p.value()["normal_map"].get<std::string>();

				std::cout << "primitive:" << std::endl;
				std::cout << "   obj_file: " << obj_file << std::endl;
				std::cout << "   name: " << name << std::endl;
				std::cout << "   material: " << material << std::endl;
				std::cout << "   translate: " << translate << std::endl;
				std::cout << "   rotate: " << rotate << std::endl;
				std::cout << "   scale: " << scale << std::endl;
				std::cout << "   vs: " << vs << std::endl;
				std::cout << "   fs: " << fs << std::endl;
				std::cout << "   albedo_map: " << albedo_map << std::endl;
				std::cout << "   normal_map: " << normal_map << std::endl;

				m_primitives.push_back(std::make_unique<Primitive>());
				m_primitives[primitive_count]->loadGeometryFromOBJ(obj_file);
				m_primitives[primitive_count]->loadMaterialFromShaders(vs, fs, logicaldevice, swapchain, descriptorsetlayout, renderpass, msaabuffer);
				m_primitives[primitive_count]->m_translation = glm::vec3(translate[0], translate[1], translate[2]);
				m_primitives[primitive_count]->m_rotation = glm::vec3(rotate[0], rotate[1], rotate[2]);
				m_primitives[primitive_count]->m_scale = glm::vec3(scale[0], scale[1], scale[2]);
				m_primitives[primitive_count]->recalculateModelMatrix();

				m_primitives[primitive_count]->m_texture.createTextureImage(albedo_map, logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->m_texture.createTextureImageView(logicaldevice);
				m_primitives[primitive_count]->m_texture.createTextureSampler(logicaldevice, physicaldevice);
				
				m_primitives[primitive_count]->m_normalmap.createTextureImage(normal_map, logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->m_normalmap.createTextureImageView(logicaldevice);
				m_primitives[primitive_count]->m_normalmap.createTextureSampler(logicaldevice, physicaldevice);

				m_primitives[primitive_count]->createVertexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createIndexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createUniformBuffers(maxFramesinFlight, logicaldevice, physicaldevice);
				m_primitives[primitive_count]->m_descriptorsets.createDescriptorSets(maxFramesinFlight, logicaldevice, descriptorpool, descriptorsetlayout, m_primitives[primitive_count]->m_uniformbuffers, m_primitives[primitive_count]->m_texture, m_primitives[primitive_count]->m_normalmap);
				
				primitive_count++;
			}
		}
		else if (s.key() == "lights") {
			std::string obj_file = "../models/sphere.obj";
			std::string vs = "../shaders/vertshader.spv";
			std::string fs = "../shaders/light.spv";
			std::string albedo_map = "../textures/white.png";
			std::string normal_map = "../textures/norm0.png";

			for (auto p : s.value().items()) {
				auto name = p.value()["name"].get<std::string>();
				auto type = p.value()["type"].get<std::string>();
				auto lightColor = p.value()["lightColor"];
				auto transform = p.value()["transform"];
				auto translate = transform["translate"];
				auto rotate = transform["rotate"];
				auto scale = transform["scale"];

				std::cout << "light:" << std::endl;
				std::cout << "   name: " << name << std::endl;
				std::cout << "   type: " << type << std::endl;
				std::cout << "   lightColor: " << lightColor << std::endl;
				std::cout << "   translate: " << translate << std::endl;
				std::cout << "   rotate: " << rotate << std::endl;
				std::cout << "   scale: " << scale << std::endl;

				

				m_primitives.push_back(std::make_unique<Primitive>());
				m_primitives[primitive_count]->m_light = std::make_unique<PointLight>();
				m_lights.push_back(m_primitives[primitive_count]->m_light.get());
				m_primitives[primitive_count]->loadGeometryFromOBJ(obj_file);
				m_primitives[primitive_count]->loadMaterialFromShaders(vs, fs, logicaldevice, swapchain, descriptorsetlayout, renderpass, msaabuffer);
				m_primitives[primitive_count]->setScale(glm::vec3(0.05));
				m_primitives[primitive_count]->m_light->m_center = glm::vec3(translate[0], translate[1], translate[2]);
				m_primitives[primitive_count]->m_light->m_intensity = glm::vec3(lightColor[0], lightColor[1], lightColor[2]);

				m_primitives[primitive_count]->m_texture.createTextureImage(albedo_map, logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->m_texture.createTextureImageView(logicaldevice);
				m_primitives[primitive_count]->m_texture.createTextureSampler(logicaldevice, physicaldevice);

				m_primitives[primitive_count]->m_normalmap.createTextureImage(normal_map, logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->m_normalmap.createTextureImageView(logicaldevice);
				m_primitives[primitive_count]->m_normalmap.createTextureSampler(logicaldevice, physicaldevice);

				m_primitives[primitive_count]->createVertexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createIndexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createUniformBuffers(maxFramesinFlight, logicaldevice, physicaldevice);
				m_primitives[primitive_count]->m_descriptorsets.createDescriptorSets(maxFramesinFlight, logicaldevice, descriptorpool, descriptorsetlayout, m_primitives[primitive_count]->m_uniformbuffers, m_primitives[primitive_count]->m_texture, m_primitives[primitive_count]->m_normalmap);

				primitive_count++;

			}
		}
		else if (s.key() == "materials") {

		}
	}
}