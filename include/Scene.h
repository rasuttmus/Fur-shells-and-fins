#ifndef SCENE_H
#define SCENE_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "../include/Geometry.h"


class Scene {

public:

	Scene();

	~Scene();

	void initialize();

	void render();

	void addGeometry(Geometry * G) { mGeometries.push_back(G); }

private:

	// Instance varialbes

	glm::mat4 mProjectionMatrix;

	glm::mat4 mViewMatrix;

	glm::mat4 mModelMatrix;


	// Containers

	std::vector<Geometry *> mGeometries;

	std::vector<glm::mat4> mMatrices;


	// Structs

	struct LightSource {
		LightSource(const glm::vec3 & p = glm::vec3(0.0f, 0.0f, 0.0f),
					const glm::vec4 & c = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
			: pos(p),
			  color(c) {}

		glm::vec3 pos;
		glm::vec4 color;
	}mLightSource;

};

#endif // SCENE_H