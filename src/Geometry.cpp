#include "../include/Geometry.h"

Geometry::Geometry(std::vector<std::string> S, glm::vec3 c, unsigned int n, float l, bool r)
    : mNumberOfLayers(n), mFurLength(l), mShallRender(r) {

    mMaterial.color = c;

    mTextureName = S[I_TEXTURE];
    mHairMapName = S[I_HAIRMAP];

    mTextureWidth = mTextureHeight = std::stoi(S[I_TEXSIZE], nullptr, 0);

    std::string obj = PATH_OBJ + S[I_FILENAME] + FILE_NAME_OBJ;
    loadMesh(obj.c_str());
}


Geometry::~Geometry() {

    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &uvBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vertexArrayID);

    for(unsigned int i = 0; i < mFurLayers.size(); i++) {
        if(mFurLayers[i])
            delete mFurLayers[i];
    }

    std::cout << "\nGeometry destroyed!\n" << std::endl;
}


void Geometry::initialize(glm::vec3 lightPosition) {

    // Generate fur noise texture
    generateNoiseTexture();

    // Generate skin texture
    std::string skinTexturename = PATH_TEX + mTextureName + FILE_NAME_PNG;
    int skinTextureHeight, skinTextureWidth;
    skinTextureID = loadTexture(skinTexturename, skinTextureWidth, skinTextureHeight);

    std::string hairMapTexturename = PATH_TEX + mHairMapName + FILE_NAME_PNG;
    int hairMapHeight, hairMapWidth;
    hairMapID = loadTexture(hairMapTexturename, hairMapWidth, hairMapHeight);

    // Then create fur layers since they use the render data from the geometry
    createFurLayers();

    mMaterial.furColor = mFurLayers.front()->getColor();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    //shaderProgram = LoadShaders("shaders/phongvertexshader.glsl", "shaders/phongfragmentshader.glsl");

    // Bind shader variables (uniforms) to indices
    MVPLoc          = glGetUniformLocation(shaderProgram, "MVP");
    MLoc            = glGetUniformLocation(shaderProgram, "M");
    VLoc            = glGetUniformLocation(shaderProgram, "V");
    lightPosLoc     = glGetUniformLocation(shaderProgram, "lightPosition");
    cameraPosLoc    = glGetUniformLocation(shaderProgram, "cameraPosition");
    colorLoc        = glGetUniformLocation(shaderProgram, "color");
    ambientLoc      = glGetUniformLocation(shaderProgram, "ambientColor");
    diffuseLoc      = glGetUniformLocation(shaderProgram, "diffuseColor");
    specularLoc     = glGetUniformLocation(shaderProgram, "specularColor");
    transparencyLoc = glGetUniformLocation(shaderProgram, "transparency");
    specularityLoc  = glGetUniformLocation(shaderProgram, "specularity");
    shinynessLoc    = glGetUniformLocation(shaderProgram, "shinyness");
    lightPowerLoc   = glGetUniformLocation(shaderProgram, "lightPower");
    skinTextureLoc  = glGetUniformLocation(shaderProgram, "skinTextureSampler");

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

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it) {
        (*it)->setShaderProgram(furShaderProgram);
        (*it)->initialize(lightPosition);
    }

    std::cout << "\nGeometry initialized!\n";
}


void Geometry::render(std::vector<glm::mat4> matrices, float lightSourcePower, float windVelocity, glm::vec3 cameraPosition) {

    glEnable( GL_CULL_FACE );
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skinTextureID);
    glUniform1i(skinTextureLoc, 0);

    // Pass data to shaders as uniforms
    glUniformMatrix4fv(MVPLoc,          1,                       GL_FALSE,              &matrices[I_MVP][0][0]);
    glUniformMatrix4fv(MLoc,            1,                       GL_FALSE,              &matrices[I_M][0][0]);
    glUniformMatrix4fv(VLoc,            1,                       GL_FALSE,              &matrices[I_V][0][0]);
    glUniform3f(       cameraPosLoc,    cameraPosition[0],       cameraPosition[1],     cameraPosition[2]);
    glUniform3f(       colorLoc,        mMaterial.color[0],      mMaterial.color[1],    mMaterial.color[2]);
    glUniform3f(       ambientLoc,      mMaterial.ambient[0],    mMaterial.ambient[1],  mMaterial.ambient[2]);
    glUniform3f(       diffuseLoc,      mMaterial.diffuse[0],    mMaterial.diffuse[1],  mMaterial.diffuse[2]);
    glUniform3f(       specularLoc,     mMaterial.specular[0],   mMaterial.specular[1], mMaterial.specular[2]);
    glUniform1f(       transparencyLoc, mMaterial.transparency);
    glUniform1f(       specularityLoc,  mMaterial.specularity);
    glUniform1f(       shinynessLoc,    mMaterial.shinyness);
    glUniform1f(       lightPowerLoc,   lightSourcePower);

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

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->render(matrices, lightSourcePower, windVelocity, cameraPosition);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


void Geometry::updateFur(float dt) {

    float offset = 0.0f;

    float stepLength = mFurLength / static_cast<float>(mNumberOfLayers);

    for(std::vector<Layer*>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it) {
        (*it)->setOffset(offset += stepLength);
        (*it)->setFurLength(mFurLength);
        (*it)->setFurNoiseLengthVariation(mFurNoiseLengthVariation);
        (*it)->setFurNoiseSampleScale(mFurNoiseSampleScale);
        (*it)->setColor(mMaterial.furColor);
        (*it)->setFurPatternScale(mMaterial.furPatternScale);
        (*it)->setNoiseType(mNoiseType);
    }
}

void Geometry::setScreenCoordMovement(glm::vec2 m) {

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->setScreenCoordMovement(m * 0.5f);
}


void Geometry::setCurrentTime(float t) {

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->setCurrentTime(t);
}


bool Geometry::loadMesh(const char * objName) {

    if(!loadObj(objName, mRenderVerts, mRenderUvs, mRenderNormals))
        return false;
}


void Geometry::createFurLayers() {

    mFurLayers.resize(mNumberOfLayers);

    float offset = 0.0f;

    float stepLength = mFurLength / static_cast<float>(mNumberOfLayers);

    for(unsigned int i = 0; i < mNumberOfLayers; i++) {

        mFurLayers[i] = new Layer(mRenderVerts, mRenderUvs, mRenderNormals, offset += stepLength, mNumberOfLayers, i);
        mFurLayers[i]->setFurNoiseLengthVariation(mFurNoiseLengthVariation);
        mFurLayers[i]->setFurNoiseSampleScale(mFurNoiseSampleScale);
        mFurLayers[i]->setNoiseTextureID(noiseTextureID);
        mFurLayers[i]->setHairMapID(hairMapID);
        mFurLayers[i]->setHairMapName(mHairMapName);
    }
}


void Geometry::generateNoiseTexture() {

    unsigned int x = 0, y = 0;
    float noiseScale = 1.0f;

    mTextureData.resize(mTextureWidth * mTextureHeight * 4);

    for (unsigned int i = 0; i < mTextureData.size(); i+=4) {
        
        if(i%(mTextureWidth*4) == 0 && i != 0) {
            y++;
            x = 0;
        }

        mTextureData[i]   = snoise2(static_cast<float>((x * noiseScale)), static_cast<float>((y * noiseScale))) * 255.0f;
        mTextureData[i+1] = snoise2(static_cast<float>((x * noiseScale)), static_cast<float>((y * noiseScale))) * 255.0f;
        mTextureData[i+2] = snoise2(static_cast<float>((x * noiseScale)), static_cast<float>((y * noiseScale))) * 255.0f;
        mTextureData[i+3] = 255;

        x++;
    }

    // Generate OpenGL texture
    glGenTextures(1, &noiseTextureID);
    glBindTexture(GL_TEXTURE_2D, noiseTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, &mTextureData[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


GLuint Geometry::loadTexture(const std::string filename, int &width, int &height) {

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