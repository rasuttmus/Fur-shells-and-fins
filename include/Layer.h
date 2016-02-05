#ifndef LAYER_H
#define LAYER_H

#define TEXTURE_LOAD_ERROR 0

#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <png.h>
#include <cstdio>

#include "utils/Util.h"
#include "utils/Shader.h"
#include "utils/Simplexnoise1234.h"
#include "utils/TGAloader.h"


class Layer {

public:

	Layer(std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>, float, unsigned int, unsigned int, glm::vec3 c = glm::vec3(0.71, 0.55, 0.34));

	~Layer();

	void initialize(glm::vec3, glm::vec3);

	void render(std::vector<glm::mat4>, float);

    void update(float);

    void generateTexture(std::vector<GLubyte>, int, int);

    glm::vec3 getColor()                     { return mMaterial.color; }

    void setOffset(float o)                  { mOffset = o; }

    void setFurLength(float l)               { mFurLength = l; }

    void setFurNoiseLengthVariation(float v) { mFurNoiseLengthVariation = v; }

    void setFurNoiseSampleScale(float s)     { mFurNoiseSampleScale = s; }

    void setColor(glm::vec3 c)               { mMaterial.color = c; }

    void setScreenCoordMovement(glm::vec2 m) { mScreenCoordMovement = m; }

    void setCurrentTime(float t)             { mCurrentTime = t; }

    void setHairMapID(GLuint t)              { hairMapID = t; }

private:

    void createTextureData();

    GLuint loadTexture(const std::string filename, int &width, int &height);

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

    float mFurLength;

    float mFurNoiseLengthVariation;

    float mFurNoiseSampleScale;

    float mCurrentTime = 0.0f;

    unsigned int mNumberOfLayers;

    unsigned int mIndex;

    glm::mat4 mRotationMatrix   = glm::mat4(1.0);

    glm::vec2 mScreenCoordMovement = glm::vec2(0.0f, 0.0f);

    Texture texture;


	// Indices for shader stuff: arrays, buffers and programs

    GLuint vertexArrayID;

    GLuint vertexBuffer;

    GLuint uvBuffer;

    GLuint normalBuffer;

    GLuint noiseTextureID;

    GLuint hairMapID;

    GLuint skinTextureID;

    GLuint noiseTextureLoc;

    GLuint hairMapLoc;

    GLuint skinTextureLoc;

    GLuint shaderProgram;


    // Uniform indices

    GLint MVPLoc;

    GLint MVLoc;

    GLint MLoc;

    GLint VLoc;
    
    GLint MVLightLoc;
    
    GLint NMLoc;

    GLint RLoc;
    
    GLint lightPosLoc;

    GLint cameraPosLoc;
    
    GLint colorLoc;
    
    GLint ambientLoc;
    
    GLint diffuseLoc;
   
    GLint transparencyLoc;

    GLint lightPowerLoc;

    GLint offsetLoc;

    GLint furLengthLoc;

    GLint numberOfLayersLoc;

    GLint layerIndexLoc;

    GLint furNoiseLengthVariationLoc;

    GLint furNoiseSampleScaleLoc;

    GLint currentTimeLoc;


	// Containers

	std::vector<glm::vec3> mRenderVerts;

	std::vector<glm::vec2> mRenderUvs;

	std::vector<glm::vec3> mRenderNormals;

    std::vector<GLubyte> mTextureData;
};

#endif // LAYER_H