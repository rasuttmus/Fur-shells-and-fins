#include "../include/Camera.h"

Camera::Camera(glm::vec3 p)
	: mPosition(p) {

}


Camera::~Camera() {

}


void Camera::initialize() {

	// Set M, V and P matrices for the camera
	mProjectionMatrix = glm::perspective(
							45.0f,			// Field of view
							4.0f / 3.0f,	// Aspect ratio
							0.1f,			// Near clipping plane
							100.0f			// Far clipping plane
						);

	mViewMatrix 	  = glm::lookAt(
							mPosition,						// Camera position in world coordinates
							glm::vec3(0.0f, 0.0f, 0.0f),	// Look-at position
							glm::vec3(0.0f, 1.0f, 0.0f)		// Up vector
						);

	mModelMatrix	  = glm::mat4(1.0f);
}