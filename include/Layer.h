#ifndef LAYER_H
#define LAYER_H

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/Util.h"
#include "utils/Shader.h"


class Layer {

public:

	Layer(std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>, float, glm::vec3 c = glm::vec3(0.71, 0.55, 0.34));

	~Layer();

	void initialize(glm::vec3, glm::vec3);

	void render(std::vector<glm::mat4>, float);

private:

	// Structs

	struct Material {
        Material(const glm::vec3 & c = glm::vec3(1.0f, 1.0f, 1.0f),
                 const glm::vec3 & a = glm::vec3(0.3f, 0.3f, 0.3f),
                 const glm::vec3 & d = glm::vec3(0.8f, 0.8f, 0.8f),
                 float t             = 0.1f)
            : color(c),
              ambient(a),
              diffuse(d),
              transparency(t) {}

        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float transparency;
    } mMaterial;

	// Instance variables

	float mOffset;

	// Indices for shader stuff: arrays, buffers and programs

    GLuint vertexArrayID;

    GLuint vertexBuffer;

    GLuint uvBuffer;

    GLuint normalBuffer;

    GLuint shaderProgram;

    // Uniform indices

    GLint MVPLoc;

    GLint MVLoc;

    GLint MLoc;

    GLint VLoc;
    
    GLint MVLightLoc;
    
    GLint NMLoc;
    
    GLint lightPosLoc;

    GLint cameraPosLoc;
    
    GLint colorLoc;
    
    GLint ambientLoc;
    
    GLint diffuseLoc;
   
    GLint transparencyLoc;

    GLint lightPowerLoc;

    GLint offsetLoc;

	// Containers

	std::vector<glm::vec3> mRenderVerts;

	std::vector<glm::vec2> mRenderUvs;

	std::vector<glm::vec3> mRenderNormals;

};

#endif // LAYER_H