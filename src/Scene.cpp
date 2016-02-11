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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up the camera
	mCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	// Light source position
	mLightSource.pos = glm::vec3(0.0f, 5.0f, 0.0f);

	GLuint phongID = LoadShaders(mShaderPrograms[I_PHONG].first.c_str(), mShaderPrograms[I_PHONG].second.c_str());
	GLuint furID   = LoadShaders(mShaderPrograms[I_FUR].first.c_str(),   mShaderPrograms[I_FUR].second.c_str());

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it) {
		(*it)->setShaderProgram(phongID);
		(*it)->setFurShaderProgram(furID);
		(*it)->initialize(mLightSource.pos);
	}

	std::cout << "\nScene initialized!\n";
}


void Scene::render() {

	mCamera->update();

	mMatrices.resize(6);

	// Get all matrices from the camera so that they can be assigned to each object when rendering

	mMatrices[I_MVP] 	  = mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * mCamera->getModelMatrix();

	mMatrices[I_M] 	  	  = mCamera->getModelMatrix();

	mMatrices[I_V] 	  	  = mCamera->getViewMatrix();

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it) {
		if((*it)->getShallRender())
			(*it)->render(mMatrices, mLightSource.power, mWindVelocity, mCamera->getPosition());
	}
}


void Scene::update(float dt) {

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it) {
		(*it)->updateFur(dt);
	}
}


void Scene::updateCameraPosition(double x, double y) {
    
    if(!mCamera->dragged())
        return;

    mCamera->rotate(mCamera->getOrientation(), x, y);

    mCamera->dragUpdate(x, y);

    for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
    	(*it)->setScreenCoordMovement(mCamera->getScreenCoordMovement());
}


void Scene::updateCameraZoom(double x, double y) {
    
    if((3.0 + (mCamera->getZoom() - y / 5.0f)) > 0.1f)
        mCamera->setZoom(mCamera->getZoom() - (y / 5.0f));
}


void Scene::resetCamera() {

	mCamera->reset();
}


void Scene::setCurrentTime(float t) {

	for(std::vector<Geometry *>::iterator it = mGeometries.begin(); it != mGeometries.end(); ++it)
		(*it)->setCurrentTime(t);
}
