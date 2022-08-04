#include "camera.h"

Camera::Camera() :	m_forward(), m_right(), m_up(), m_ref(), m_eye(), 
					m_width(), m_height(), m_aspect(), m_fovy(), m_near(), m_far(), 
					m_theta(), m_phi(), m_radius() 
{}

Camera::~Camera() {}

void Camera::recalculateAspectRatio() {
	m_aspect = (float)m_width / (float)m_height;
}