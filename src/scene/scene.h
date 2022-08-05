#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "../render/renderpass.h"
#include "../image/msaabuffer.h"
#include "../device/logicaldevice.h"

#include "primitive.h"

class Scene {
public:
	Scene();
	~Scene();



	void loadSceneFromJSON(std::string& filePath);

	// scene data
	std::vector<std::unique_ptr<Primitive>>			m_primitives;


private:

};


#endif
