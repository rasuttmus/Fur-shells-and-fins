#include "../include/Geometry.h"

Geometry::Geometry(std::string s, glm::vec3 c, unsigned int n, float l)
    : mNumberOfLayers(n), mFurLength(l) {

    mMaterial.color = c;

    std::string obj = PATH_OBJ + s + FILE_NAME_OBJ;
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


void Geometry::initialize(glm::vec3 lightPosition, glm::vec3 cameraPosition) {

    // Generate fur noise texture
    generateTexture();

    // Generate skin texture
    std::string skinTexturename = "assets/textures/monkey_tex.png";
    int skinTextureHeight, skinTextureWidth;
    skinTextureID = loadTexture(skinTexturename, skinTextureWidth, skinTextureHeight);

    // Then create fur layers since they use the render data from the geometry
    createFurLayers();

    mMaterial.furColor = mFurLayers.front()->getColor();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders("shaders/phongvertexshader.glsl", "shaders/phongfragmentshader.glsl");

    // Bind shader variables (uniforms) to indices
    MVPLoc          = glGetUniformLocation(shaderProgram, "MVP");
    MVLoc           = glGetUniformLocation(shaderProgram, "MV");
    MLoc            = glGetUniformLocation(shaderProgram, "M");
    VLoc            = glGetUniformLocation(shaderProgram, "V");
    MVLightLoc      = glGetUniformLocation(shaderProgram, "MVLight");
    NMLoc           = glGetUniformLocation(shaderProgram, "NM");
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

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->initialize(lightPosition, cameraPosition);

    std::cout << "\nGeometry initialized!\n";
}


void Geometry::render(std::vector<glm::mat4> matrices, float lightSourcePower) {

    glEnable( GL_CULL_FACE );
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skinTextureID);
    glUniform1i(skinTextureLoc, 0);

    // Pass data to shaders as uniforms
    glUniformMatrix4fv(MVPLoc,          1,                       GL_FALSE,              &matrices[I_MVP][0][0]);
    glUniformMatrix4fv(MVLoc,           1,                       GL_FALSE,              &matrices[I_MV][0][0]);
    glUniformMatrix4fv(MLoc,            1,                       GL_FALSE,              &matrices[I_M][0][0]);
    glUniformMatrix4fv(VLoc,            1,                       GL_FALSE,              &matrices[I_V][0][0]);
    glUniformMatrix4fv(MVLightLoc,      1,                       GL_FALSE,              &matrices[I_MV_LIGHT][0][0]);
    glUniformMatrix4fv(NMLoc,           1,                       GL_FALSE,              &matrices[I_NM][0][0]);
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
        (*it)->render(matrices, lightSourcePower);

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
        (*it)->update(dt);
    }
}

void Geometry::setScreenCoordMovement(glm::vec2 m) {

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->setScreenCoordMovement(m*0.5f);
}


void Geometry::setCurrentTime(float t) {

    for(std::vector<Layer *>::iterator it = mFurLayers.begin(); it != mFurLayers.end(); ++it)
        (*it)->setCurrentTime(t);
}


bool Geometry::loadMesh(const char * objName) {

    if(!loadObj(objName, mRenderVerts, mRenderUvs, mRenderNormals))
        return false;
}


bool Geometry::addFace(
    std::vector<glm::vec3> verts,
    std::vector<glm::vec2> uvs,
    std::vector<glm::vec3> normals
    ) {

    unsigned int vertIndex1, vertIndex2, vertIndex3, uvIndex1, uvIndex2, uvIndex3;

    // Add vertices and uvs
    addVertex(verts[0], normals[0], vertIndex1);
    addUv(uvs[0], uvIndex1, vertIndex1);

    addVertex(verts[1], normals[1], vertIndex2);
    addUv(uvs[1], uvIndex2, vertIndex2);

    addVertex(verts[2], normals[2], vertIndex3);
    addUv(uvs[2], uvIndex3, vertIndex3);

    // Create face and add properties
    Face face;
    face.v1 = vertIndex1;
    face.v2 = vertIndex2;
    face.v3 = vertIndex3;
    mFaces.push_back(face);
    mFaces.back().normal = faceNormal(mFaces.size()-1);
}


bool Geometry::addVertex(const glm::vec3 &vertex, glm::vec3 normal, unsigned int &vertIndex) {

    // Search the map of unique vertices for the vertex that we try to add
    std::map<glm::vec3, unsigned int>::iterator it = mUniqueVerts.find(vertex);

    // If the vertex already exists, return its index
    if(it != mUniqueVerts.end()) {
        vertIndex = (*it).second;
        return false;
    }

    // The vertex does not exist, add it!
    // Set index to size of vertex list, i.e. place it in the back
    mUniqueVerts[vertex] = vertIndex = getNumVerts();

    Vertex vert;
    vert.pos = vertex;
    vert.normal = normal;
    mVerts.push_back(vert);

    return true;
}


bool Geometry::addUv(const glm::vec2 &uv, unsigned int &uvIndex, unsigned int vertIndex) {

    // Search the map of unique vertices for the vertex that we try to add
    std::map<glm::vec2, unsigned int>::iterator it = mUniqueUvs.find(uv);

    // If the uv already exists, return its index
    if(it != mUniqueUvs.end()) {
        uvIndex = (*it).second;
        return false;
    }

    // The uv does not exist, add it!
    // Set index to size of uv list, i.e. place it in the back
    mUniqueUvs[uv] = uvIndex = getNumUvs();

    mVerts[vertIndex].uv = uv;
    mUvs.push_back(uv);

    return true;
}


glm::vec3 Geometry::faceNormal(unsigned int faceIndex) {

    // Compute two edges in the triangle
    glm::vec3 edge1 = mVerts[mFaces[faceIndex].v2].pos - mVerts[mFaces[faceIndex].v1].pos;
    glm::vec3 edge2 = mVerts[mFaces[faceIndex].v3].pos - mVerts[mFaces[faceIndex].v1].pos;

    // Return the normalized cross product
    return glm::normalize(glm::cross(edge1, edge2));
}


void Geometry::buildRenderData() {

    unsigned int index = 0;

    for(unsigned int i = 0; i < mFaces.size(); i++) {

        mRenderVerts.push_back(mVerts[mFaces[i].v1].pos);
        mRenderVerts.push_back(mVerts[mFaces[i].v2].pos);
        mRenderVerts.push_back(mVerts[mFaces[i].v3].pos);

        mRenderUvs.push_back(mVerts[mFaces[i].v1].uv);
        mRenderUvs.push_back(mVerts[mFaces[i].v2].uv);
        mRenderUvs.push_back(mVerts[mFaces[i].v3].uv);

        mRenderNormals.push_back(mVerts[mFaces[i].v1].normal);
        mRenderNormals.push_back(mVerts[mFaces[i].v2].normal);
        mRenderNormals.push_back(mVerts[mFaces[i].v3].normal);
    }
}


void Geometry::createFurLayers() {

    mFurLayers.resize(mNumberOfLayers);

    float offset = 0.0f;

    float stepLength = mFurLength / static_cast<float>(mNumberOfLayers);

    for(unsigned int i = 0; i < mNumberOfLayers; i++) {

        mFurLayers[i] = new Layer(mRenderVerts, mRenderUvs, mRenderNormals, offset += stepLength, mNumberOfLayers, i);
        mFurLayers[i]->setFurNoiseLengthVariation(mFurNoiseLengthVariation);
        mFurLayers[i]->setFurNoiseSampleScale(mFurNoiseSampleScale);
        mFurLayers[i]->generateTexture(mTextureData, mTextureWidth, mTextureHeight);
    }
}


void Geometry::generateTexture() {

    mTextureWidth = mTextureHeight = 1024;
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