#include "../include/Scene.h"

Scene::Scene() {

}


Scene::~Scene() {

	for(unsigned int i = 0; i < mGeometries.size(); i++) {
		if(mGeometries[i])
			delete mGeometries[i];
	}

	std::cout << "Scene destroyed!\n" << std::endl;
}


void Scene::initialize() {
	
	std::cout << "\nInitializing Scene...";

	// Background color
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Light source position
	mLightSource.pos = glm::vec3(0.0f, 4.0f, 0.0f);

	// Set M, V and P matrices for the scene
	mProjectionMatrix = glm::perspective(
							45.0f,			// Field of view
							4.0f / 3.0f,	// Aspect ratio
							0.1f,			// Near clipping plane
							100.0f			// Far clipping plane
						);

	mViewMatrix 	  = glm::lookAt(
							glm::vec3(0.0f, 0.0f, 3.0f),	// Camera position in world coordinates
							glm::vec3(0.0f, 0.0f, 0.0f),	// Look-at position
							glm::vec3(0.0f, 1.0f, 0.0f)		// Up vector
						);

	mModelMatrix	  = glm::mat4(1.0f);

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
		(*it)->initialize(mLightSource.pos);

	std::cout << "Done!\n";
}


void Scene::render() {

	mMatrices.resize(4);

	mMatrices[I_MVP] 	  = mProjectionMatrix * mViewMatrix * mModelMatrix;

	mMatrices[I_MV] 	  = mViewMatrix * mModelMatrix;

	mMatrices[I_MV_LIGHT] = mViewMatrix * mModelMatrix;

	mMatrices[I_NM]		  = glm::inverseTranspose(glm::mat4(mViewMatrix * mModelMatrix));


	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
		(*it)->render(mMatrices);

}
