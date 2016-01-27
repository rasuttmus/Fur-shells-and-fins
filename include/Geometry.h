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

    Geometry(std::string, glm::vec3 c = glm::vec3(1.0f, 1.0f, 1.0f), unsigned int n = 4, float l = 0.5f);

    ~Geometry();

    void initialize(glm::vec3, glm::vec3);

    void render(std::vector<glm::mat4>, float);

    glm::vec3 &getColor()        { return mMaterial.color; }

    glm::vec3 &getAmbient()      { return mMaterial.ambient; }

    glm::vec3 &getDiffuse()      { return mMaterial.diffuse; }

    glm::vec3 &getSpecular()     { return mMaterial.specular; }

    float     &getSpecularity()  { return mMaterial.specularity; }

    float     &getTransparency() { return mMaterial.transparency; }

    float     &getShinyness()    { return mMaterial.shinyness; }

private:

    // Functions

    bool loadMesh(const char *);

    bool addFace(std::vector<glm::vec3>, std::vector<glm::vec2>, std::vector<glm::vec3>);

    bool addVertex(const glm::vec3 &, glm::vec3, unsigned int &);

    bool addUv(const glm::vec2 &, unsigned int &, unsigned int);

    glm::vec3 faceNormal(unsigned int);

    unsigned int getNumVerts() const { return mVerts.size(); }

    unsigned int getNumUvs() const { return mUvs.size(); }

    void buildRenderData();

    void createFurLayers();


    // Structs

    struct Face {
        Face(const glm::vec3 & n = glm::vec3(0.0f, 0.0f, 0.0f)) 
            : normal(n),
              v1(UNINITIALIZED),
              v2(UNINITIALIZED),
              v3(UNINITIALIZED) {}

        glm::vec3 normal;
        unsigned int v1;
        unsigned int v2;
        unsigned int v3;
    };

    struct Vertex {
        Vertex(const glm::vec3 & p = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::vec3 & n = glm::vec3(0.0f, 0.0f, 0.0f),
               const glm::vec2 & u = glm::vec2(0.0f, 0.0f))
            : pos(p),
              normal(n),
              uv(u) {}

        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Material {
        Material(const glm::vec3 & c = glm::vec3(1.0f, 1.0f, 1.0f),
                 const glm::vec3 & a = glm::vec3(0.3f, 0.3f, 0.3f),
                 const glm::vec3 & d = glm::vec3(0.8f, 0.8f, 0.8f),
                 const glm::vec3 & s = glm::vec3(1.0f, 1.0f, 1.0f),
                 float t             = 1.0f,
                 float sp            = 25.0f,
                 float sh            = 0.3f)
            : color(c),
              ambient(a),
              diffuse(d),
              specular(s),
              transparency(t),
              specularity(sp),
              shinyness(sh) {}

        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float transparency;
        float specularity;
        float shinyness;
    } mMaterial;


    // Instance variables

    glm::mat4 modelMatrix;

    unsigned int mNumberOfLayers;

    float mFurLength;


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
    
    GLint specularLoc;

    GLint transparencyLoc;
    
    GLint specularityLoc;
    
    GLint shinynessLoc;

    GLint lightPowerLoc;


    // Containers

    std::map<glm::vec3, unsigned int> mUniqueVerts;

    std::map<glm::vec2, unsigned int> mUniqueUvs;

    std::vector<Face> mFaces;

    std::vector<Vertex> mVerts;

    std::vector<glm::vec2> mUvs;

    std::vector<glm::vec3> mRenderVerts;

    std::vector<glm::vec2> mRenderUvs;

    std::vector<glm::vec3> mRenderNormals;

    std::vector<Layer *> mFurLayers;
};

#endif // GEOMETRY_H