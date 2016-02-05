#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "../include/utils/Util.h"


class Camera {

public:

	Camera(glm::vec3 p = glm::vec3(0.0f, 0.0f, 3.0f));

	~Camera();

	void update();

	glm::quat& rotate(glm::quat &, double, double);

    glm::vec2 direction(double x, double y);

    void dragStart(double, double);
    
    void dragUpdate(double, double);
    
    void dragEnd();
    
    bool dragged() const 				  { return mDragged; }

    void center(float x, float y) 		  { mCenterPosition.x = x; mCenterPosition.y = y; }

    void reset();

	glm::mat4 getModelMatrix() 			  { return mModelMatrix; }

	glm::mat4 getViewMatrix() 			  { return mViewMatrix; }

	glm::mat4 getProjectionMatrix() 	  { return mProjectionMatrix; }

	glm::vec3 getPosition() 			  { return mPosition; }

	glm::quat& getOrientation() 	   	  { return mOrientation; }

	float getZoom() 					  { return mZoom; }

	glm::vec2 getScreenCoordMovement() 	  { return mScreenCoordDifference; }

	void setModelMatrix(glm::mat4 M) 	  { mModelMatrix = M; }

	void setViewMatrix(glm::mat4 V) 	  { mViewMatrix = V; }

	void setProjectionMatrix(glm::mat4 P) { mProjectionMatrix = P; }

	void setOrientation(glm::quat O) 	  { mOrientation = O; }

	void setZoom(float z) 				  { mZoom = z; }

private:

	// Functions

	glm::vec3 map_to_sphere(const glm::vec2 &point);


	// Instance variables

	glm::mat4 mModelMatrix;

	glm::mat4 mViewMatrix;

	glm::mat4 mProjectionMatrix;

	glm::quat mOrientation;

	glm::vec3 mPosition;

	glm::vec2 mCenterPosition;
    
    glm::vec2 mDragStartPosition;
    
    float mRadius;

    float mZoom;
    
    bool mDragged;

    bool mFirstFrame = true;

    glm::vec2 mPreviousScreenCoord;

    glm::vec2 mScreenCoordDifference;
};

#endif // CAMERA_H