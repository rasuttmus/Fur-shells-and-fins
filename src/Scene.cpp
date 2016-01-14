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

	// Background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Set up the camera
	mCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	// Light source position
	mLightSource.pos = glm::vec3(0.0f, 5.0f, 0.0f);

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
		(*it)->initialize(mLightSource.pos, mCamera->getPosition());

	std::cout << "\nScene initialized!\n";
}


void Scene::render() {

	mCamera->update();

	mMatrices.resize(6);

	// Get all matrices from the camera so that they can be assigned to each object when rendering

	mMatrices[I_MVP] 	  = mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * mCamera->getModelMatrix();

	mMatrices[I_MV] 	  = mCamera->getViewMatrix() * mCamera->getModelMatrix();

	mMatrices[I_M] 	  	  = mCamera->getModelMatrix();

	mMatrices[I_V] 	  	  = mCamera->getViewMatrix();

	mMatrices[I_MV_LIGHT] = mCamera->getViewMatrix() * mCamera->getModelMatrix();

	mMatrices[I_NM]		  = glm::inverseTranspose(glm::mat4(mCamera->getViewMatrix() * mCamera->getModelMatrix()));

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
		(*it)->render(mMatrices);
}


void Scene::updateCameraPosition(double x, double y) {
    
    if(!mCamera->dragged())
        return;

    mCamera->rotate(mCamera->getOrientation(), x, y);
    mCamera->dragUpdate(x, y);
}


void Scene::updateCameraZoom(double x, double y) {
    
    if((3.0 + (mCamera->getZoom() - y / 5.0f)) > 0.1f)
        mCamera->setZoom(mCamera->getZoom() - (y / 5.0f));
}


void Scene::resetCamera() {

	mCamera->reset();
}
