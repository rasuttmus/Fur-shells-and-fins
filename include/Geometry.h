#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/Util.h"
#include "utils/ObjectLoader.h"
#include "utils/Shader.h"
#include "../include/Layer.h"


class Geometry {

public:

    Geometry(std::vector<std::string>, glm::vec3 c = glm::vec3(1.0f, 1.0f, 1.0f), unsigned int n = 4, float l = 0.5f, bool r = true);

    ~Geometry();

    void      initialize(glm::vec3, glm::vec3);

    void      render(std::vector<glm::mat4>, float, float);

    void      updateFur(float);

    GLuint    loadTexturePNG(const std::string, int &, int &);

    glm::vec3 getColor()                           { return mMaterial.color; }

    glm::vec3 getFurColor()                        { return mMaterial.furColor; }

    glm::vec3 getAmbient()                         { return mMaterial.ambient; }

    glm::vec3 getDiffuse()                         { return mMaterial.diffuse; }

    glm::vec3 getSpecular()                        { return mMaterial.specular; }

    float     getSpecularity()                     { return mMaterial.specularity; }

    float     getTransparency()                    { return mMaterial.transparency; }

    float     getShinyness()                       { return mMaterial.shinyness; }

    float     getFurPatternScale()                 { return mMaterial.furPatternScale; }

    float     getFurLength()                       { return mFurLength; }

    float     getFurNoiseLengthVariation()         { return mFurNoiseLengthVariation; }

    float     getFurNoiseSampleScale()             { return mFurNoiseSampleScale; }

    bool      getShallRender()                     { return mShallRender; }

    int       getNoiseType()                       { return mNoiseType; }

    void      setScreenCoordMovement(glm::vec2);

    void      setCurrentTime(float t);

    void      setShallRender(bool r)               { mShallRender = r; }

    void      setFurLength(float l)                { mFurLength = l; }

    void      setColor(glm::vec3 c)                { mMaterial.color = c; }

    void      setAmbient(glm::vec3 a)              { mMaterial.ambient = a; }

    void      setDiffuse(glm::vec3 d)              { mMaterial.diffuse = d; }

    void      setSpecular(glm::vec3 s)             { mMaterial.specular = s; }

    void      setFurColor(glm::vec3 f)             { mMaterial.furColor = f; }

    void      setSpecularity(float sp)             { mMaterial.specularity = sp; }

    void      setTransparency(float tr)            { mMaterial.transparency = tr; }

    void      setShinyness(float sh)               { mMaterial.shinyness = sh; }

    void      setFurPatternScale(float ps)         { mMaterial.furPatternScale = ps; }

    void      setFurNoiseLengthVariation(float lv) { mFurNoiseLengthVariation = lv; }

    void      setFurNoiseSampleScale(float ss)     { mFurNoiseSampleScale = ss; }

    void      setNoiseType(int nt)                 { mNoiseType = nt; }

private:

    // Functions

    bool loadMesh(const char *);

    void createFurLayers();

    void generateTexture();

    GLuint loadTexture(const std::string filename, int &width, int &height);


    // Structs

    struct Material {
        Material(const glm::vec3 & c = glm::vec3(1.0f, 1.0f, 1.0f),
                 const glm::vec3 & f = glm::vec3(1.0f, 1.0f, 1.0f),
                 const glm::vec3 & a = glm::vec3(0.3f, 0.3f, 0.3f),
                 const glm::vec3 & d = glm::vec3(0.8f, 0.8f, 0.8f),
                 const glm::vec3 & s = glm::vec3(1.0f, 1.0f, 1.0f),
                 float t             = 1.0f,
                 float sp            = 25.0f,
                 float sh            = 0.3f,
                 float ps            = 4.0f)
            : color(c),
              furColor(f),
              ambient(a),
              diffuse(d),
              specular(s),
              transparency(t),
              specularity(sp),
              shinyness(sh),
              furPatternScale(ps) {}

        glm::vec3 color;
        glm::vec3 furColor;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float transparency;
        float specularity;
        float shinyness;
        float furPatternScale;
    } mMaterial;


    // Instance variables

    unsigned int mNumberOfLayers;

    float mFurLength;

    float mFurNoiseLengthVariation = 0.2f;

    float mFurNoiseSampleScale = 1.5f;

    int mTextureWidth;

    int mTextureHeight;

    glm::vec3 mRotation;

    std::string mTextureName;

    std::string mHairMapName;

    bool mShallRender;

    int mNoiseType;


    // Indices for shader stuff: arrays, buffers and programs

    GLuint vertexArrayID;

    GLuint vertexBuffer;

    GLuint uvBuffer;

    GLuint normalBuffer;

    GLuint shaderProgram;

    GLuint skinTextureID;
    
    GLuint skinTextureLoc;


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
    
    GLint specularLoc;

    GLint transparencyLoc;
    
    GLint specularityLoc;
    
    GLint shinynessLoc;

    GLint lightPowerLoc;


    // Containers

    //std::map<glm::vec3, unsigned int> mUniqueVerts;

    //std::map<glm::vec2, unsigned int> mUniqueUvs;

    //std::vector<Face> mFaces;

    //std::vector<Vertex> mVerts;

    //std::vector<glm::vec2> mUvs;

    std::vector<glm::vec3> mRenderVerts;

    std::vector<glm::vec2> mRenderUvs;

    std::vector<glm::vec3> mRenderNormals;

    std::vector<Layer *> mFurLayers;

    std::vector<GLubyte> mTextureData;
};

#endif // GEOMETRY_H