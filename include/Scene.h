#ifndef SCENE_H
#define SCENE_H


#include "../include/Geometry.h"
#include "../include/Camera.h"


class Scene {

public:

	Scene();

	~Scene();

	void   initialize();

	void   render();

	void   update(float);

	void   addGeometry(Geometry * G) 			  { mGeometries.push_back(G); }

	void   updateCameraPosition(double, double);

    void   updateCameraZoom(double, double);
    
    void   mousePress(double x, double y) 	   	  { mCamera->dragStart(x, y); }
    
    void   mouseRelease() 					      { mCamera->dragEnd(); }

    void   resetCamera();

    float &getLightSourcePower() 			      { return mLightSource.power; }

    void   setCurrentTime(float t);

private:

	// Instance varialbes

	Camera * mCamera = nullptr;


	// Containers

	std::vector<Geometry *> mGeometries;

	std::vector<glm::mat4> mMatrices;


	// Structs

	struct LightSource {
		LightSource(const glm::vec3 & p = glm::vec3(0.0f, 0.0f, 0.0f),
					const glm::vec4 & c = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
					const float pow 	= 1.0f)
			: pos(p),
			  color(c),
			  power(pow) {}

		glm::vec3 pos;
		glm::vec4 color;
		float power;

	} mLightSource;

};

#endif // SCENE_H