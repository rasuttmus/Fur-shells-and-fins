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


void Layer::initialize(glm::vec3 lightPosition, glm::vec3 cameraPosition) {

    // Generate hairmap
    std::string hairMapName = PATH_TEX + mHairMapName + FILE_NAME_PNG;
    int hairMapHeight, hairMapWidth;
    hairMapID = loadTexture(hairMapName, hairMapWidth, hairMapHeight);

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders("shaders/furvertexshader.glsl", "shaders/furfragmentshader.glsl");

    // Bind shader variables (uniforms) to indices
    MVPLoc                     = glGetUniformLocation(shaderProgram, "MVP");
    MVLoc                      = glGetUniformLocation(shaderProgram, "MV");
    MLoc                       = glGetUniformLocation(shaderProgram, "M");
    VLoc                       = glGetUniformLocation(shaderProgram, "V");
    MVLightLoc                 = glGetUniformLocation(shaderProgram, "MVLight");
    NMLoc                      = glGetUniformLocation(shaderProgram, "NM");
    RLoc                       = glGetUniformLocation(shaderProgram, "PhysicalRotationMatrix");
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


    glUniform3f(lightPosLoc,  lightPosition[0],  lightPosition[1],  lightPosition[2]);
    glUniform3f(cameraPosLoc, cameraPosition[0], cameraPosition[1], cameraPosition[2]);

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


void Layer::render(std::vector<glm::mat4> matrices, float lightSourcePower, float windVelocity) {

    glUseProgram(shaderProgram);

    // Set active tex-unit and bind texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTextureID);
    glUniform1i(noiseTextureLoc, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, hairMapID);
    glUniform1i(hairMapLoc, 2);

    float rotationScaleFactor = (float)pow(((float)mIndex / (float)mNumberOfLayers), 3.0f) *  1.0f;
    mRotationMatrix = glm::rotate(glm::mat4(1.0), static_cast<float>(-mScreenCoordMovement.x * M_PI / 180.0f) * rotationScaleFactor, glm::vec3(0.0f, 1.0f, 0.0f));
    mRotationMatrix = glm::rotate(mRotationMatrix, static_cast<float>(-mScreenCoordMovement.y * M_PI / 180.0f) * rotationScaleFactor, glm::vec3(1.0f, 0.0f, 0.0f));

    matrices[I_MVP] = matrices[I_MVP] * mRotationMatrix;
    matrices[I_MV] = matrices[I_MV] * mRotationMatrix;
    matrices[I_MV] = matrices[I_M] * mRotationMatrix;
    matrices[I_MV_LIGHT] = matrices[I_MV_LIGHT] * mRotationMatrix;

    // Pass data to shaders as uniforms
    glUniformMatrix4fv(MVPLoc,                     1,                        GL_FALSE,              &matrices[I_MVP][0][0]);
    glUniformMatrix4fv(MVLoc,                      1,                        GL_FALSE,              &matrices[I_MV][0][0]);
    glUniformMatrix4fv(MLoc,                       1,                        GL_FALSE,              &matrices[I_M][0][0]);
    glUniformMatrix4fv(VLoc,                       1,                        GL_FALSE,              &matrices[I_V][0][0]);
    glUniformMatrix4fv(MVLightLoc,                 1,                        GL_FALSE,              &matrices[I_MV_LIGHT][0][0]);
    glUniformMatrix4fv(NMLoc,                      1,                        GL_FALSE,              &matrices[I_NM][0][0]);
    glUniformMatrix4fv(RLoc,                       1,                        GL_FALSE,              &mRotationMatrix[0][0]);
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


void Layer::update(float dt) {

}


void Layer::generateTexture(std::vector<GLubyte> textureData, int w, int h) {

    // Generate OpenGL texture
    glGenTextures(1, &noiseTextureID);
    glBindTexture(GL_TEXTURE_2D, noiseTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, &textureData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Layer::createTextureData() {

    int w, h;
    w = h = 512;
    unsigned int x = 0, y = 0;
    float noiseScale = 1.0f;

    mTextureData.resize(w * h * 4);

    for (unsigned int i = 0; i < mTextureData.size(); i+=4) {
        
        if(i%(w*4) == 0 && i != 0) {
            y++;
            x = 0;
        }

        if(y > 128 && y < 256) {
            mTextureData[i]   = 255;
            mTextureData[i+1] = 255;
            mTextureData[i+2] = 255;
            mTextureData[i+3] = 255;
        } else {
            mTextureData[i]   = 0;
            mTextureData[i+1] = 0;
            mTextureData[i+2] = 0;
            mTextureData[i+3] = 255;
        }

        x++;
    }

    // Generate OpenGL texture
    glGenTextures(1, &hairMapID);
    glBindTexture(GL_TEXTURE_2D, hairMapID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, &mTextureData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


GLuint Layer::loadTexture(const std::string filename, int &width, int &height) {

   //header for testing if it is a png
   png_byte header[8];
   
   //open file as binary
   FILE *fp = fopen(filename.c_str(), "rb");
   if (!fp) {
     return TEXTURE_LOAD_ERROR;
   }
   
   //read the header
   fread(header, 1, 8, fp);
   
   //test if png
   int is_png = !png_sig_cmp(header, 0, 8);
   if (!is_png) {
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   
   //create png struct
   png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
       NULL, NULL);
   if (!png_ptr) {
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
   
   //create png info struct
   png_infop info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr) {
     png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
 
   //create png info struct
   png_infop end_info = png_create_info_struct(png_ptr);
   if (!end_info) {
     png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
 
   //png error stuff, not sure libpng man suggests this.
   if (setjmp(png_jmpbuf(png_ptr))) {
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return (TEXTURE_LOAD_ERROR);
   }
 
   //init png reading
   png_init_io(png_ptr, fp);
   
   //let libpng know you already read the first 8 bytes
   png_set_sig_bytes(png_ptr, 8);
 
   // read all the info up to the image data
   png_read_info(png_ptr, info_ptr);
 
   //variables to pass to get info
   int bit_depth, color_type;
   png_uint_32 twidth, theight;
 
   // get info about png
   png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
       NULL, NULL, NULL);
 
   //update width and height based on png info
   width = twidth;
   height = theight;
 
   // Update the png info struct.
   png_read_update_info(png_ptr, info_ptr);
 
   // Row size in bytes.
   int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
 
   // Allocate the image_data as a big block, to be given to opengl
   png_byte *image_data = new png_byte[rowbytes * height];
   if (!image_data) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
 
   //row_pointers is for pointing to image_data for reading the png with libpng
   png_bytep *row_pointers = new png_bytep[height];
   if (!row_pointers) {
     //clean up memory and close stuff
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return TEXTURE_LOAD_ERROR;
   }
   // set the individual row_pointers to point at the correct offsets of image_data
   for (int i = 0; i < height; ++i)
     row_pointers[height - 1 - i] = image_data + i * rowbytes;
 
   //read the png into image_data through row_pointers
   png_read_image(png_ptr, row_pointers);
 
   //Now generate the OpenGL texture object
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0,
       GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 
   //clean up memory and close stuff
   png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
   delete[] image_data;
   delete[] row_pointers;
   fclose(fp);
 
   return texture;
}