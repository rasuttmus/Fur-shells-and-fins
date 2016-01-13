#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


class Camera {

public:

	Camera(glm::vec3 p = glm::vec3(0.0f, 0.0f, 3.0f));

	~Camera();

	void initialize();

	glm::mat4 getModelMatrix() { return mModelMatrix; };

	glm::mat4 getViewMatrix() { return mViewMatrix; };

	glm::mat4 getProjectionMatrix() { return mProjectionMatrix; };

	glm::vec3 getPosition() { return mPosition; };

private:

	glm::mat4 mModelMatrix;

	glm::mat4 mViewMatrix;

	glm::mat4 mProjectionMatrix;

	glm::vec3 mPosition;

};

#endif // CAMERA_H