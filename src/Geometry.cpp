#include "../include/Geometry.h"

Geometry::Geometry(std::string s) {

	std::string obj = PATH_OBJ + s + FILE_NAME_OBJ;

	loadMesh(obj.c_str());
}


Geometry::~Geometry() {

	std::cout << "\nGeometry destroyed!\n" << std::endl;
}


void Geometry::initialize(glm::vec3 lightPosition) {

	buildRenderData();

	glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
}


void Geometry::render(std::vector<glm::mat4> matrices) {

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

	/*for(unsigned int i = 0; i < mVerts.size(); i++)
		std::cout << "\nvert index " << i << ": " << mVerts[i].pos.x << ", " << mVerts[i].pos.y << ", " << mVerts[i].pos.z << std::endl;

	for(unsigned int i = 0; i < mVerts.size(); i++)
		std::cout << "\nnormal vert index " << i << ": " << mVerts[i].normal.x << ", " << mVerts[i].normal.y << ", " << mVerts[i].normal.z << std::endl;

	for(unsigned int i = 0; i < mUvs.size(); i++)
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

		mRenderUvs.push_back(mVerts[mFaces[i].v1].uv);
		mRenderUvs.push_back(mVerts[mFaces[i].v2].uv);
		mRenderUvs.push_back(mVerts[mFaces[i].v3].uv);

		mRenderNormals.push_back(mVerts[mFaces[i].v1].normal);
		mRenderNormals.push_back(mVerts[mFaces[i].v2].normal);
		mRenderNormals.push_back(mVerts[mFaces[i].v3].normal);
	}
}
