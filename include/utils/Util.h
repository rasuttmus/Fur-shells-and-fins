/*
 *	Helper file for global utility variables
 */
#ifndef UTIL_H
#define UTIL_H

// Indices for matrices in the scene and data of the geometry
#define I_MVP 		0
#define I_M  		1
#define I_V  		2

#define I_FILENAME	0
#define I_TEXTURE	1
#define I_HAIRMAP	2
#define I_TEXSIZE	3

#define I_PHONG		0
#define I_FUR		1

#include <glm/vec3.hpp>

// Overloaded operators from glm
// This is need in order to use glm and stl stuff together
namespace glm {
	template <typename T, precision P>
	bool operator<(const tvec3<T, P>& a,const tvec3<T, P>& b) {

		if(a.x < b.x || (a.y < b.y || a.z < b.z)) return true;
		if(((a.x == b.x && a.y == b.y) && a.z < b.z)) return true;
		if((a.x == b.x && a.z == b.z) && a.y < b.y) return true;
		if((a.y == b.y && a.z == b.z) && a.x < b.x) return true;
		return false;
  	}

  	template <typename T, precision P>
	bool operator<(const tvec2<T, P>& a,const tvec2<T, P>& b) {

		if(a.x < b.x || (a.x == b.x && a.y < b.y)) return true;
		return false;
  	}
}

// Constants
const std::string PATH_OBJ = "assets/";
const std::string PATH_TEX = "assets/textures/";
const std::string FILE_NAME_OBJ = ".obj";
const std::string FILE_NAME_PNG = ".png";

const static unsigned int UNINITIALIZED = (std::numeric_limits<unsigned int>::max)();

const int WIDTH = 1024;
const int HEIGHT = 768;

typedef enum { SIMPLEX, WORLEY } NoiseType;

#endif // UTIL_H