#include "camera.h"

Camera::Camera() :	m_forward(), m_right(), m_up(), m_ref(), m_eye(), 
					m_width(), m_height(), m_aspect(), m_fovy(), m_near(), m_far(), 
					m_theta(), m_phi(), m_radius(), m_view_projection()
{}

Camera::~Camera() {}

void Camera::recalculateAspectRatio() {
	m_aspect = (float)m_width / (float)m_height;
	recalculateViewProjection();
}

void Camera::recalculateViewProjection() {
	glm::mat4 view = glm::lookAt(m_eye, m_ref, m_up);
	glm::mat4 proj = glm::perspective(m_fovy, m_aspect, m_near, m_far);
	proj[1][1] *= -1;
	m_view_projection = proj * view;
}