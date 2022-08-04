#ifndef CAMERA_H
#define CAMERA_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

class Camera {
public:
	Camera();
	~Camera();

	// orientation vectors
	glm::vec3		m_forward;
	glm::vec3		m_right;
	glm::vec3		m_up;
	glm::vec3		m_ref; // reference point

	// position vector
	glm::vec3		m_eye;

	// perspective parameters
	int				m_width;
	int				m_height;
	float			m_aspect;
	float			m_fovy;
	float			m_near;
	float			m_far;

	// for polar-spherical camera movement
	float			m_theta, m_phi;   // Degrees along the sphere the camera is positioned
	float			m_radius;       // Radius through the sphere the camera is positioned

	void recalculateAspectRatio();

private:

};


#endif
