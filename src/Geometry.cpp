#include "../include/Geometry.h"

Geometry::Geometry(std::string s, glm::vec3 c) {

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

	std::cout << "\nGeometry destroyed!\n" << std::endl;
}


void Geometry::initialize(glm::vec3 lightPosition, glm::vec3 cameraPosition) {

	buildRenderData();

	glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders("shaders/phongvertexshader.glsl", "shaders/phongfragmentshader.glsl");

    // Bind shader variables (uniforms) to indices
    MVPLoc 	     	= glGetUniformLocation(shaderProgram, "MVP");
    MVLoc        	= glGetUniformLocation(shaderProgram, "MV");
    MLoc        	= glGetUniformLocation(shaderProgram, "M");
    VLoc        	= glGetUniformLocation(shaderProgram, "V");
    MVLightLoc   	= glGetUniformLocation(shaderProgram, "MVLight");
    NMLoc	     	= glGetUniformLocation(shaderProgram, "NM");
    lightPosLoc  	= glGetUniformLocation(shaderProgram, "lightPosition");
    cameraPosLoc  	= glGetUniformLocation(shaderProgram, "cameraPosition");
    colorLoc 	 	= glGetUniformLocation(shaderProgram, "color");
    ambientLoc   	= glGetUniformLocation(shaderProgram, "ambientColor");
    diffuseLoc   	= glGetUniformLocation(shaderProgram, "diffuseColor");
    specularLoc     = glGetUniformLocation(shaderProgram, "specularColor");
    transparencyLoc = glGetUniformLocation(shaderProgram, "transparency");
    specularityLoc  = glGetUniformLocation(shaderProgram, "specularity");
    shinynessLoc	= glGetUniformLocation(shaderProgram, "shinyness");

    glUniform3f(lightPosLoc, lightPosition[0], lightPosition[1], lightPosition[2]);
    glUniform3f(cameraPosLoc, cameraPosition[0], cameraPosition[1], cameraPosition[2]);


    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderVerts.size() * sizeof(glm::vec3), &mRenderVerts[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
    	0,								// shader layout, in this case 0
    	3,								// size, 3 for vec3
    	GL_FLOAT,						// type, float in vec3
    	GL_FALSE,						// normalized, nope
    	0,								// stride, 0
    	reinterpret_cast<void*>(0)		// array buffer offset, none
    );


    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderUvs.size() * sizeof(glm::vec2), &mRenderUvs[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
    	1,								// shader layout, in this case 1
    	2,								// size, 2 for vec2
    	GL_FLOAT,						// type, float in vec2
    	GL_FALSE,						// normalized, no
    	0,								// stride, 0
    	reinterpret_cast<void*>(0)		// array buffer offset, no
    );

    
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mRenderNormals.size() * sizeof(glm::vec3), &mRenderNormals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
    	2,								// shader layout, in this case 2
    	3,								// size, 3 for a vec3
    	GL_FLOAT,						// type, float for vec3's
    	GL_FALSE,						// normalized, no
    	0,								// stride, 0
    	reinterpret_cast<void*>(0)		// array buffer offset, 0
    );

    std::cout << "\nGeometry initialized!\n";
}


void Geometry::render(std::vector<glm::mat4> matrices) {

	/*glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	matrices[I_MVP] = matrices[I_MVP] * translationMatrix;
	matrices[I_MV] = matrices[I_MV] * translationMatrix;
	matrices[I_M] = matrices[I_M] * translationMatrix;
	matrices[I_V] = matrices[I_V] * translationMatrix;
	//matrices[I_MV_LIGHT] = matrices[I_MV_LIGHT] * translationMatrix;
	matrices[I_NM] = matrices[I_NM] * translationMatrix;
*/
	glUseProgram(shaderProgram);

	// Pass data to shaders as uniforms
	glUniformMatrix4fv(MVPLoc, 	   1, GL_FALSE, &matrices[I_MVP][0][0]);
	glUniformMatrix4fv(MVLoc, 	   1, GL_FALSE, &matrices[I_MV][0][0]);
	glUniformMatrix4fv(MLoc, 	   1, GL_FALSE, &matrices[I_M][0][0]);
	glUniformMatrix4fv(VLoc, 	   1, GL_FALSE, &matrices[I_V][0][0]);
	glUniformMatrix4fv(MVLightLoc, 1, GL_FALSE, &matrices[I_MV_LIGHT][0][0]);
	glUniformMatrix4fv(NMLoc, 	   1, GL_FALSE, &matrices[I_NM][0][0]);
	glUniform3f(colorLoc, 	 mMaterial.color[0],	mMaterial.color[1],    mMaterial.color[2]);
	glUniform3f(ambientLoc,  mMaterial.ambient[0],  mMaterial.ambient[1],  mMaterial.ambient[2]);
	glUniform3f(diffuseLoc,  mMaterial.diffuse[0],  mMaterial.diffuse[1],  mMaterial.diffuse[2]);
	glUniform3f(specularLoc, mMaterial.specular[0], mMaterial.specular[1], mMaterial.specular[2]);
	glUniform1f(transparencyLoc, mMaterial.transparency);
	glUniform1f(specularityLoc,  mMaterial.specularity);
	glUniform1f(shinynessLoc, 	 mMaterial.shinyness);

	
	// Rebind vertex, uv, and normal data, since everything is updated every frame
	glBindVertexArray(vertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, mRenderVerts.size() 	* sizeof(glm::vec3), &mRenderVerts[0],   GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, mRenderUvs.size() 	* sizeof(glm::vec2), &mRenderUvs[0], 	 GL_STATIC_DRAW);

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


bool Geometry::loadMesh(const char * objName) {

	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	if(!loadObj(objName, verts, uvs, normals))
		return false;

	std::vector<glm::vec3> v;
	std::vector<glm::vec2> u;
	std::vector<glm::vec3> n;

	v.resize(3);
	u.resize(3);
	n.resize(3);

	for(unsigned int i = 0; i < verts.size(); i+=3) {
		
		v[0] = verts[i];
		v[1] = verts[i+1];
		v[2] = verts[i+2];

		u[0] = uvs[i];
		u[1] = uvs[i+1];
		u[2] = uvs[i+2];

		n[0] = normals[i];
		n[1] = normals[i+1];
		n[2] = normals[i+2];

		addFace(v, u, n);
	}

/*	for(unsigned int i = 0; i < mVerts.size(); i++)
		std::cout << "\nvert index " << i << ": " << mVerts[i].pos.x << ", " << mVerts[i].pos.y << ", " << mVerts[i].pos.z << std::endl;*/

/*	for(unsigned int i = 0; i < mVerts.size(); i++)
		std::cout << "\nnormal vert index " << i << ": " << mVerts[i].normal.x << ", " << mVerts[i].normal.y << ", " << mVerts[i].normal.z << std::endl;*/

/*	for(unsigned int i = 0; i < mUvs.size(); i++)
		std::cout << "\nuv index " << i << ": " << mUvs[i].x << ", " << mUvs[i].y << std::endl;*/
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

		//std::cout << "vert1: " << mVerts[mFaces[i].v1].pos.x << ", " << mVerts[mFaces[i].v1].pos.y << ", " << mVerts[mFaces[i].v1].pos.z << std::endl;
		//std::cout << "vert2: " << mVerts[mFaces[i].v2].pos.x << ", " << mVerts[mFaces[i].v2].pos.y << ", " << mVerts[mFaces[i].v2].pos.z << std::endl;
		//std::cout << "vert3: " << mVerts[mFaces[i].v3].pos.x << ", " << mVerts[mFaces[i].v3].pos.y << ", " << mVerts[mFaces[i].v3].pos.z << std::endl;

		mRenderUvs.push_back(mVerts[mFaces[i].v1].uv);
		mRenderUvs.push_back(mVerts[mFaces[i].v2].uv);
		mRenderUvs.push_back(mVerts[mFaces[i].v3].uv);

		//std::cout << "uv1: " << mVerts[mFaces[i].v1].uv.x << ", " << mVerts[mFaces[i].v1].uv.y << std::endl;
		//std::cout << "uv2: " << mVerts[mFaces[i].v2].uv.x << ", " << mVerts[mFaces[i].v2].uv.y << std::endl;
		//std::cout << "uv3: " << mVerts[mFaces[i].v3].uv.x << ", " << mVerts[mFaces[i].v3].uv.y << std::endl;

		mRenderNormals.push_back(mVerts[mFaces[i].v1].normal);
		mRenderNormals.push_back(mVerts[mFaces[i].v2].normal);
		mRenderNormals.push_back(mVerts[mFaces[i].v3].normal);

		//std::cout << "normal1: " << mVerts[mFaces[i].v1].normal.x << ", " << mVerts[mFaces[i].v1].normal.y << ", " << mVerts[mFaces[i].v1].normal.z << std::endl;
		//std::cout << "normal2: " << mVerts[mFaces[i].v2].normal.x << ", " << mVerts[mFaces[i].v2].normal.y << ", " << mVerts[mFaces[i].v2].normal.z << std::endl;
		//std::cout << "normal3: " << mVerts[mFaces[i].v3].normal.x << ", " << mVerts[mFaces[i].v3].normal.y << ", " << mVerts[mFaces[i].v3].normal.z << std::endl;
	}
}
