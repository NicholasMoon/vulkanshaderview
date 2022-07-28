#ifndef MESH_H
#define MESH_H


#include <tiny_obj_loader.h>

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "../vertex.h"

class Mesh {
public:
	Mesh();
	Mesh(std::string& filePath);
	~Mesh();

	void loadOBJ(std::string& filePath);
	void toString();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
private:
	std::string fileName;
	
};

#endif