#include "scene.h"

Scene::Scene() : m_camera() {}

Scene::~Scene() {}

void Scene::createMaterialFromShaderFiles(std::string& vs, std::string& fs, LogicalDevice& logicaldevice, Swapchain& swapchain, RenderPass& renderpass, MSAABuffer& msaabuffer) {
	m_materials.push_back(std::make_unique<Material>());
	int thisMaterial = m_materials.size() - 1;

	m_materials[thisMaterial]->m_shader.create(vs, fs, logicaldevice);

	m_materials[thisMaterial]->m_pipeline.createGraphicsPipeline(m_materials[thisMaterial]->m_shader, swapchain, logicaldevice, m_materials[thisMaterial]->m_shader.descriptorsetlayout, renderpass, msaabuffer);
}

void Scene::loadSceneFromJSON(std::string& filePath, LogicalDevice& logicaldevice, PhysicalDevice& physicaldevice, Swapchain& swapchain, CommandPool& commandpool, DescriptorPool& descriptorpool, RenderPass& renderpass, MSAABuffer& msaabuffer, uint32_t maxFramesinFlight) {
	std::ifstream f(filePath);
	json j = json::parse(f);

	int primitive_count = 0;
	int mesh_count = 0;
	int shader_count = 0;

	std::unordered_map<std::string, int> OBJfiles;
	std::unordered_map<std::string, int> Shaderfiles;

    auto sceneData = j["scene"];
	for (auto s : sceneData.items()) {
		if (s.key() == "camera") {
			auto ref = s.value()["ref"];
			auto eye = s.value()["eye"];
			auto up = s.value()["worldUp"];
			auto fovy = s.value()["fov"];
			auto width = s.value()["width"];
			auto height = s.value()["height"];

			/*std::cout << "camera:" << std::endl;
			std::cout << "   ref: " << ref << std::endl;
			std::cout << "   eye: " << eye << std::endl;
			std::cout << "   up: " << up << std::endl;
			std::cout << "   fovy: " << fovy << std::endl;
			std::cout << "   width: " << width << std::endl;
			std::cout << "   height: " << height << std::endl;*/

			m_camera.m_eye = glm::vec3(eye[0], eye[1], eye[2]);
			m_camera.m_ref = glm::vec3(ref[0], ref[1], ref[2]);
			m_camera.m_up = glm::vec3(up[0], up[1], up[2]);
			m_camera.m_fovy = glm::radians(fovy.get<float>());
			m_camera.m_width = width;
			m_camera.m_height = height;
			m_camera.recalculateAspectRatio();
			m_camera.m_near = 0.1f;
			m_camera.m_far = 30.0f;
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

				/*std::cout << "primitive:" << std::endl;
				std::cout << "   obj_file: " << obj_file << std::endl;
				std::cout << "   name: " << name << std::endl;
				std::cout << "   material: " << material << std::endl;
				std::cout << "   translate: " << translate << std::endl;
				std::cout << "   rotate: " << rotate << std::endl;
				std::cout << "   scale: " << scale << std::endl;
				std::cout << "   vs: " << vs << std::endl;
				std::cout << "   fs: " << fs << std::endl;
				std::cout << "   albedo_map: " << albedo_map << std::endl;
				std::cout << "   normal_map: " << normal_map << std::endl;*/

				m_primitives.push_back(std::make_unique<Primitive>());

				if (OBJfiles.count(obj_file) == 0) {
					m_meshes.push_back(std::make_unique<Mesh>());
					m_meshes[mesh_count]->loadOBJ(obj_file);
					OBJfiles[obj_file] = mesh_count;
					m_primitives[primitive_count]->m_geometry = m_meshes[mesh_count].get();
					mesh_count++;
				}
				else {
					m_primitives[primitive_count]->m_geometry = m_meshes[OBJfiles[obj_file]].get();
				}

				// add new material only if shader hasn't been created before (NOTE: only checks fs for now)
				if (Shaderfiles.count(fs) == 0) {
					createMaterialFromShaderFiles(vs, fs, logicaldevice, swapchain, renderpass, msaabuffer);
					Shaderfiles[fs] = shader_count;
					m_primitives[primitive_count]->m_material = m_materials[shader_count].get();
					shader_count++;
				}
				else {
					m_primitives[primitive_count]->m_material = m_materials[Shaderfiles[fs]].get();
				}
				

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
				m_primitives[primitive_count]->m_descriptorsets.createDescriptorSets(maxFramesinFlight, logicaldevice, descriptorpool, m_primitives[primitive_count]->m_material->m_shader.descriptorsetlayout, m_primitives[primitive_count]->m_uniformbuffers, m_primitives[primitive_count]->m_texture, m_primitives[primitive_count]->m_normalmap);
				
				primitive_count++;
			}
		}
		else if (s.key() == "lights") {
			std::string obj_file = "../models/light.obj";
			std::string vs = "../shaders/vertshader_light.spv";
			std::string fs = "../shaders/light.spv";

			for (auto p : s.value().items()) {
				auto name = p.value()["name"].get<std::string>();
				auto type = p.value()["type"].get<std::string>();
				auto lightColor = p.value()["lightColor"];
				auto transform = p.value()["transform"];
				auto translate = transform["translate"];
				auto rotate = transform["rotate"];
				auto scale = transform["scale"];

				/*std::cout << "light:" << std::endl;
				std::cout << "   name: " << name << std::endl;
				std::cout << "   type: " << type << std::endl;
				std::cout << "   lightColor: " << lightColor << std::endl;
				std::cout << "   translate: " << translate << std::endl;
				std::cout << "   rotate: " << rotate << std::endl;
				std::cout << "   scale: " << scale << std::endl;*/

				

				m_primitives.push_back(std::make_unique<Primitive>());

				if (OBJfiles.count(obj_file) == 0) {
					m_meshes.push_back(std::make_unique<Mesh>());
					m_meshes[mesh_count]->loadOBJ(obj_file);
					OBJfiles[obj_file] = mesh_count;
					m_primitives[primitive_count]->m_geometry = m_meshes[mesh_count].get();
					mesh_count++;
				}
				else {
					m_primitives[primitive_count]->m_geometry = m_meshes[OBJfiles[obj_file]].get();
				}

				m_primitives[primitive_count]->m_light = std::make_unique<PointLight>();
				m_lights.push_back(m_primitives[primitive_count]->m_light.get());

				// add new material only if shader hasn't been created before (NOTE: only checks fs for now)
				if (Shaderfiles.count(fs) == 0) {
					createMaterialFromShaderFiles(vs, fs, logicaldevice, swapchain, renderpass, msaabuffer);
					Shaderfiles[fs] = shader_count;
					m_primitives[primitive_count]->m_material = m_materials[shader_count].get();
					shader_count++;
				}
				else {
					m_primitives[primitive_count]->m_material = m_materials[Shaderfiles[fs]].get();
				}

				m_primitives[primitive_count]->setScale(glm::vec3(0.05));
				m_primitives[primitive_count]->m_light->m_center = glm::vec3(translate[0], translate[1], translate[2]);
				m_primitives[primitive_count]->m_light->m_intensity = glm::vec3(lightColor[0], lightColor[1], lightColor[2]);

				m_primitives[primitive_count]->createVertexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createIndexBuffer(logicaldevice, physicaldevice, commandpool);
				m_primitives[primitive_count]->createUniformBuffers(maxFramesinFlight, logicaldevice, physicaldevice);
				m_primitives[primitive_count]->m_descriptorsets.createDescriptorSets_light(maxFramesinFlight, logicaldevice, descriptorpool, m_primitives[primitive_count]->m_material->m_shader.descriptorsetlayout, m_primitives[primitive_count]->m_uniformbuffers);

				primitive_count++;

			}
		}
		else if (s.key() == "materials") {

		}
	}
}
