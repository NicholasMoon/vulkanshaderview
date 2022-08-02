#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include "geometry/mesh.h"
#include "../image/texture.h"
#include "../databuffer/databuffer.h"
#include "../core/descriptorsets.h"
#include "light/light.h"

class Texture;
class DataBuffer;
class DescriptorSets;

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


	void loadGeometryFromOBJ(std::string& filePath);
	void toString();

	Mesh										m_geometry; // for now, don't support instancing/reusing these fields...
	Light										m_light;

	Texture										m_texture;    // allocated on a per-primitive basis
	Texture										m_normalmap;
	DataBuffer									m_vertexbuffer;
	DataBuffer									m_indexbuffer;
	std::vector<DataBuffer>						m_uniformbuffers;
	DescriptorSets								m_descriptorsets;

private:

};


#endif
