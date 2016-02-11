#include "../include/Camera.h"

Camera::Camera(glm::vec3 p)
	: mPosition(p) {

	mRadius = 300.0f;
	mCenterPosition = glm::vec2(WIDTH / 2.0f, HEIGHT / 2.0f);
	mZoom = 4.0f;
}


Camera::~Camera() {

}


void Camera::update() {

	// Set M, V and P matrices for the camera
	mProjectionMatrix = glm::perspective(
							45.0f,			// Field of view
							4.0f / 3.0f,	// Aspect ratio
							0.1f,			// Near clipping plane
							100.0f			// Far clipping plane
						);

	mViewMatrix 	  = glm::lookAt(
							glm::vec3(mPosition.x, mPosition.y, mPosition.z + mZoom),	// Camera position in world coordinates
							glm::vec3(0.0f, 0.0f, 0.0f),								// Look-at position
							glm::vec3(0.0f, 1.0f, 0.0f)									// Up vector
						) * glm::mat4_cast(mOrientation);								// Multiply with orientation to apply rotation

	mModelMatrix	  = glm::mat4(1.0f);
}


glm::quat& Camera::rotate(glm::quat &orientation, double x, double y) {

	if(!mDragged)
        return orientation;

    glm::vec3 v0 = map_to_sphere(mDragStartPosition);
    glm::vec3 v1 = map_to_sphere(glm::vec2(x - mCenterPosition.x, y - mCenterPosition.y));
    glm::vec3 v2 = glm::cross(v0, v1); // get which axis we should rotate around.

    float d = glm::dot(v0, v1);
    float s = sqrtf((1.0f + d) * 2.0f);
    glm::quat q(0.5f * s, v2 / s);

    orientation = q * orientation; // apply rotation
    orientation /= glm::length(orientation); // normalize
    
    return orientation;
}


glm::vec2 Camera::direction(double x, double y) {
    
    glm::vec2 dragEndPosition(x - mCenterPosition.x, y - mCenterPosition.y);
    glm::vec2 v(dragEndPosition.x - mDragStartPosition.x, dragEndPosition.y - mDragStartPosition.y);
    v.y = -v.y;

    return glm::normalize(v);
}


void Camera::dragStart(double x, double y) {

    mDragged = true;
    dragUpdate(x, y);
}


void Camera::dragUpdate(double x, double y) {
    
    if(mDragged) {

        mDragStartPosition.x = x - mCenterPosition.x;
        mDragStartPosition.y = y - mCenterPosition.y;

        if((int)x != 0 && (int)y != 0) {
            
            if(mFirstFrame) {

                mScreenCoordDifference = glm::vec2(0.0f, 0.0f);
                mPreviousScreenCoord = glm::vec2(x, y);
                mFirstFrame = false;
            }

            mScreenCoordDifference = glm::vec2(x, y) - mPreviousScreenCoord;

            mPreviousScreenCoord = glm::vec2(x,y);

        } else {

            mFirstFrame = true;
        }
    }
}


void Camera::dragEnd() {
    
    if(mDragged) {
        dragUpdate(0.0f, 0.0f);
        mDragged = false;
    }
}


void Camera::reset() {

	glm::quat identityQuat;
    mOrientation = identityQuat;
    mZoom = 4.0f;
}


glm::vec3 Camera::map_to_sphere(const glm::vec2 &point) {

    glm::vec2 p(point.x, point.y);

    p.y = -p.y;

    float safeRadius = mRadius - 1.0f; // safe radius is       

    if(glm::length(p) > safeRadius) {
        float theta = atan2(p.y, p.x);
        p.x = safeRadius * cos(theta);
        p.y = safeRadius * sin(theta);
    }

    float lengthSquared = pow(p.x, 2) + pow(p.y, 2);
    float z = sqrt(pow(mRadius, 2) - lengthSquared);
    glm::vec3 q(p.x, p.y, z);

    return glm::normalize(q / mRadius);
}
