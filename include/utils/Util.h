/*
 *	Helper file for global utility variables
 */
#ifndef UTIL_H
#define UTIL_H

// Indices for matrices in the scene
#define I_MVP 		0
#define I_MV  		1
#define I_M  		2
#define I_V  		2
#define I_MV_LIGHT  3
#define I_NM		4

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
const std::string FILE_NAME_OBJ = ".obj";

const static unsigned int UNINITIALIZED = (std::numeric_limits<unsigned int>::max)();

const int WIDTH = 1024;
const int HEIGHT = 768;

#endif // UTIL_H