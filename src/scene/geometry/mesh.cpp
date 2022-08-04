#include "mesh.h"

Mesh::Mesh() : fileName("") {};
Mesh::Mesh(std::string& filePath) : fileName(filePath) {
    loadOBJ(fileName);
}


void Mesh::loadOBJ(std::string &filePath) {
    fileName = filePath;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::vector<float> same_vertex_counts;

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const tinyobj::shape_t& shape : shapes) {
        //for (const tinyobj::index_t& index : shape.mesh.indices) {
        for (int i = 0; i < shape.mesh.indices.size(); i += 3) {
            std::array<Vertex, 3> tri_vertices{};
            for (int k = 0; k < 3; ++k) {
                Vertex vert{};

                vert.pos = {
                attrib.vertices[3 * shape.mesh.indices[i + k].vertex_index + 0],
                attrib.vertices[3 * shape.mesh.indices[i + k].vertex_index + 1],
                attrib.vertices[3 * shape.mesh.indices[i + k].vertex_index + 2]
                };

                vert.uvs = {
                    attrib.texcoords[2 * shape.mesh.indices[i + k].texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * shape.mesh.indices[i + k].texcoord_index + 1]
                };

                vert.nor = {
                    attrib.normals[3 * shape.mesh.indices[i + k].normal_index + 0],
                    attrib.normals[3 * shape.mesh.indices[i + k].normal_index + 1],
                    attrib.normals[3 * shape.mesh.indices[i + k].normal_index + 2]
                };

                tri_vertices[k] = vert;

                
            }
            glm::vec3 edge0 = tri_vertices[2].pos - tri_vertices[1].pos;
            glm::vec3 edge1 = tri_vertices[1].pos - tri_vertices[0].pos;
            glm::vec3 face_normal = glm::cross(edge1, edge0);

            glm::vec3 deltaPos1 = tri_vertices[1].pos - tri_vertices[0].pos;
            glm::vec3 deltaPos2 = tri_vertices[2].pos - tri_vertices[0].pos;
            glm::vec2 deltaUV1 = tri_vertices[1].uvs - tri_vertices[0].uvs;
            glm::vec2 deltaUV2 = tri_vertices[2].uvs - tri_vertices[0].uvs;
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            
            for (int k = 0; k < 3; ++k) {
                
                
                tri_vertices[k].tan.x = f * (deltaUV2.y * deltaPos1.x - deltaUV1.y * deltaPos2.x);
                tri_vertices[k].tan.y = f * (deltaUV2.y * deltaPos1.y - deltaUV1.y * deltaPos2.y);
                tri_vertices[k].tan.z = f * (deltaUV2.y * deltaPos1.z - deltaUV1.y * deltaPos2.z);

                tri_vertices[k].bit.x = f * (-deltaUV2.x * deltaPos1.x + deltaUV1.x * deltaPos2.x);
                tri_vertices[k].bit.y = f * (-deltaUV2.x * deltaPos1.y + deltaUV1.x * deltaPos2.y);
                tri_vertices[k].bit.z = f * (-deltaUV2.x * deltaPos1.z + deltaUV1.x * deltaPos2.z);

                tri_vertices[k].tan = glm::normalize(tri_vertices[k].tan);
                tri_vertices[k].bit = glm::normalize(tri_vertices[k].bit);

                if (uniqueVertices.count(tri_vertices[k]) == 0) {
                    uniqueVertices[tri_vertices[k]] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(tri_vertices[k]);
                    same_vertex_counts.push_back(1);
                }
                else {
                    same_vertex_counts[uniqueVertices[tri_vertices[k]]] += 1.0f;
                    vertices[uniqueVertices[tri_vertices[k]]].tan += tri_vertices[k].tan;
                    vertices[uniqueVertices[tri_vertices[k]]].bit += tri_vertices[k].bit;
                }

                indices.push_back(uniqueVertices[tri_vertices[k]]);
            }
        }
    }
    std::cout << vertices.size() << std::endl;
    if (vertices.size() < 200) {
        for (int i = 0; i < vertices.size(); ++i) {
            vertices[i].tan /= same_vertex_counts[i];
            vertices[i].bit /= same_vertex_counts[i];
            /*std::cout << "=========" << std::endl;
            std::cout << same_vertex_counts[i] << std::endl;
            std::cout << vertices[i].nor.x << " " << vertices[i].nor.y << " " << vertices[i].nor.z << " " << std::endl;
            std::cout << vertices[i].tan.x << " " << vertices[i].tan.y << " " << vertices[i].tan.z << " " << std::endl;
            std::cout << vertices[i].bit.x << " " << vertices[i].bit.y << " " << vertices[i].bit.z << " " << std::endl;*/
        }
    }
}

void Mesh::toString() {
	std::cout << fileName << std::endl;
}


Mesh::~Mesh() {};