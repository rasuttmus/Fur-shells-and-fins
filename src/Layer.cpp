#include "../include/Layer.h"

Layer::Layer(std::vector<glm::vec3> V, std::vector<glm::vec2> U, std::vector<glm::vec3> N, float o, glm::vec3 c)
	: mRenderVerts(V), mRenderUvs(U), mRenderNormals(N), mOffset(o) {

	mMaterial.color = c;
}


Layer::~Layer() {

}


void Layer::initialize(glm::vec3 lightPosition, glm::vec3 cameraPosition) {

	glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    shaderProgram = LoadShaders("shaders/furvertexshader.glsl", "shaders/furfragmentshader.glsl");

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
    transparencyLoc = glGetUniformLocation(shaderProgram, "transparency");
    lightPowerLoc   = glGetUniformLocation(shaderProgram, "lightPower");
    offsetLoc		= glGetUniformLocation(shaderProgram, "layerOffset");

    glUniform3f(lightPosLoc,  lightPosition[0],  lightPosition[1],  lightPosition[2]);
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
}


void Layer::render(std::vector<glm::mat4> matrices, float lightSourcePower) {

	//glEnable( GL_CULL_FACE );
    //glEnable(GL_DEPTH_TEST);

	glUseProgram(shaderProgram);

	// Pass data to shaders as uniforms
	glUniformMatrix4fv(MVPLoc, 	   		1, 						 GL_FALSE, 				&matrices[I_MVP][0][0]);
	glUniformMatrix4fv(MVLoc, 	   		1, 						 GL_FALSE, 				&matrices[I_MV][0][0]);
	glUniformMatrix4fv(MLoc, 	   		1, 						 GL_FALSE, 				&matrices[I_M][0][0]);
	glUniformMatrix4fv(VLoc, 	   		1, 						 GL_FALSE, 				&matrices[I_V][0][0]);
	glUniformMatrix4fv(MVLightLoc, 		1, 						 GL_FALSE, 				&matrices[I_MV_LIGHT][0][0]);
	glUniformMatrix4fv(NMLoc, 	   		1, 						 GL_FALSE, 				&matrices[I_NM][0][0]);
	glUniform3f(	   colorLoc, 	    mMaterial.color[0],	     mMaterial.color[1],    mMaterial.color[2]);
	glUniform3f( 	   ambientLoc,      mMaterial.ambient[0],    mMaterial.ambient[1],  mMaterial.ambient[2]);
	glUniform3f( 	   diffuseLoc,      mMaterial.diffuse[0],    mMaterial.diffuse[1],  mMaterial.diffuse[2]);
	glUniform1f( 	   transparencyLoc, mMaterial.transparency);
	glUniform1f(	   lightPowerLoc,   lightSourcePower);
	glUniform1f(	   offsetLoc,	    mOffset);

	
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

	//glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
}