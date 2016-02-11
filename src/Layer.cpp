#include "../include/Layer.h"

Layer::Layer(std::vector<glm::vec3> V, 
             std::vector<glm::vec2> U, 
             std::vector<glm::vec3> N, 
             float o, 
             unsigned int n, 
             unsigned int i, 
             glm::vec3 c)
    : mRenderVerts(V), 
      mRenderUvs(U), 
      mRenderNormals(N), 
      mOffset(o),
      mNumberOfLayers(n),
      mIndex(i) {

    mMaterial.color = c;
}


Layer::~Layer() {

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vertexArrayID);
}


void Layer::initialize(glm::vec3 lightPosition) {

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Bind shader variables (uniforms) to indices
    MVPLoc                     = glGetUniformLocation(shaderProgram, "MVP");
    MLoc                       = glGetUniformLocation(shaderProgram, "M");
    VLoc                       = glGetUniformLocation(shaderProgram, "V");
    lightPosLoc                = glGetUniformLocation(shaderProgram, "lightPosition");
    cameraPosLoc               = glGetUniformLocation(shaderProgram, "cameraPosition");
    colorLoc                   = glGetUniformLocation(shaderProgram, "color");
    ambientLoc                 = glGetUniformLocation(shaderProgram, "ambientColor");
    diffuseLoc                 = glGetUniformLocation(shaderProgram, "diffuseColor");
    transparencyLoc            = glGetUniformLocation(shaderProgram, "transparency");
    lightPowerLoc              = glGetUniformLocation(shaderProgram, "lightPower");
    offsetLoc                  = glGetUniformLocation(shaderProgram, "layerOffset");
    furLengthLoc               = glGetUniformLocation(shaderProgram, "furLength");
    numberOfLayersLoc          = glGetUniformLocation(shaderProgram, "numberOfLayers");
    layerIndexLoc              = glGetUniformLocation(shaderProgram, "layerIndex");
    noiseTextureLoc            = glGetUniformLocation(shaderProgram, "textureSampler");
    hairMapLoc                 = glGetUniformLocation(shaderProgram, "hairMapSampler");
    furNoiseLengthVariationLoc = glGetUniformLocation(shaderProgram, "furNoiseLengthVariation");
    furNoiseSampleScaleLoc     = glGetUniformLocation(shaderProgram, "furNoiseSampleScale");
    currentTimeLoc             = glGetUniformLocation(shaderProgram, "currentTime");
    windVelocityLoc            = glGetUniformLocation(shaderProgram, "windVelocity");
    furPatternScaleLoc         = glGetUniformLocation(shaderProgram, "furPatternScale");
    noiseTypeLoc               = glGetUniformLocation(shaderProgram, "noiseType");


    glUniform3f(lightPosLoc,  lightPosition[0],  lightPosition[1],  lightPosition[2]);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderVerts.size() * sizeof(glm::vec3), &mRenderVerts[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                              // shader layout, in this case 0
        3,                              // size, 3 for vec3
        GL_FLOAT,                       // type, float in vec3
        GL_FALSE,                       // normalized, nope
        0,                              // stride, 0
        reinterpret_cast<void*>(0)      // array buffer offset, none
    );


    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderUvs.size() * sizeof(glm::vec2), &mRenderUvs[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                              // shader layout, in this case 1
        2,                              // size, 2 for vec2
        GL_FLOAT,                       // type, float in vec2
        GL_FALSE,                       // normalized, no
        0,                              // stride, 0
        reinterpret_cast<void*>(0)      // array buffer offset, no
    );

    
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderNormals.size() * sizeof(glm::vec3), &mRenderNormals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,                              // shader layout, in this case 2
        3,                              // size, 3 for a vec3
        GL_FLOAT,                       // type, float for vec3's
        GL_FALSE,                       // normalized, no
        0,                              // stride, 0
        reinterpret_cast<void*>(0)      // array buffer offset, 0
    );
}


void Layer::render(std::vector<glm::mat4> matrices, float lightSourcePower, float windVelocity, glm::vec3 cameraPosition) {

    float rotationScaleFactor = (float)pow(((float)mIndex / (float)mNumberOfLayers), 3.0f);
    mRotationMatrix = glm::rotate(glm::mat4(1.0), static_cast<float>(-mScreenCoordMovement.x * M_PI / 180.0f) * rotationScaleFactor, glm::vec3(0.0f, 1.0f, 0.0f));
    mRotationMatrix = glm::rotate(mRotationMatrix, static_cast<float>(-mScreenCoordMovement.y * M_PI / 180.0f) * rotationScaleFactor, glm::vec3(1.0f, 0.0f, 0.0f));

    matrices[I_MVP] = matrices[I_MVP] * mRotationMatrix;
    matrices[I_M]   = matrices[I_M]   * mRotationMatrix;

    // Pass data to shaders as uniforms
    glUniformMatrix4fv(MVPLoc,                     1,                        GL_FALSE,              &matrices[I_MVP][0][0]);
    glUniformMatrix4fv(MLoc,                       1,                        GL_FALSE,              &matrices[I_M][0][0]);
    glUniformMatrix4fv(VLoc,                       1,                        GL_FALSE,              &matrices[I_V][0][0]);
    glUniform3f(       cameraPosLoc,               cameraPosition[0],        cameraPosition[1],     cameraPosition[2]);
    glUniform3f(       colorLoc,                   mMaterial.color[0],       mMaterial.color[1],    mMaterial.color[2]);
    glUniform3f(       ambientLoc,                 mMaterial.ambient[0],     mMaterial.ambient[1],  mMaterial.ambient[2]);
    glUniform3f(       diffuseLoc,                 mMaterial.diffuse[0],     mMaterial.diffuse[1],  mMaterial.diffuse[2]);
    glUniform1f(       transparencyLoc,            mMaterial.transparency);
    glUniform1f(       lightPowerLoc,              lightSourcePower);
    glUniform1f(       offsetLoc,                  mOffset);
    glUniform1f(       furLengthLoc,               mFurLength);
    glUniform1i(       numberOfLayersLoc,          mNumberOfLayers);
    glUniform1i(       layerIndexLoc,              mIndex);
    glUniform1f(       furNoiseLengthVariationLoc, mFurNoiseLengthVariation);
    glUniform1f(       furNoiseSampleScaleLoc,     mFurNoiseSampleScale);
    glUniform1f(       currentTimeLoc,             mCurrentTime);
    glUniform1f(       windVelocityLoc,            windVelocity);
    glUniform1f(       furPatternScaleLoc,         mFurPatternScale);
    glUniform1i(       noiseTypeLoc,               mNoiseType);


    // Rebind vertex, uv, and normal data, since everything is updated every frame
    glBindVertexArray(vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderVerts.size()   * sizeof(glm::vec3), &mRenderVerts[0],   GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderUvs.size()     * sizeof(glm::vec2), &mRenderUvs[0],     GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderNormals.size() * sizeof(glm::vec3), &mRenderNormals[0], GL_STATIC_DRAW);

    // Draw the polygons
    glDrawArrays(GL_TRIANGLES, 0, mRenderVerts.size());

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}