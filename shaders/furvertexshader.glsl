#version 330 core

// Input 
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uvCoordinate;
layout(location = 2) in vec3 vertexNormal;

uniform mat4  MVP;
uniform mat4  MV;
uniform mat4  M;
uniform mat4  V;
uniform mat4  PhysicalRotationMatrix;
uniform mat4  MVLight;
uniform vec3  lightPosition;
uniform float layerOffset;
uniform float currentTime;
uniform int   numberOfLayers;
uniform int   layerIndex;
uniform sampler2D hairMapSampler;

out vec3 normal;
out vec3 vertexPositionWorldSpace;
out vec3 vertexPositionModelSpace;
out vec3 lightDirectionCameraSpace;
out vec2 UV;
out vec3 UV3D;


//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


void main() {

	vertexPositionModelSpace = vertexPosition;

	vec3 heightSample = texture(hairMapSampler, UV).rgb;

	vec3 gravity = vec3(0.0, -9.82, 0.0);

	vec3 windDirection = vec3(0.0, 0.0, 0.0);

	windDirection.x = sin(currentTime * 3.0 + cos(snoise(vec2(currentTime, currentTime * 0.2)) * 0.5)) * 8.0;
	windDirection.y = cos(currentTime * 2.0 + sin(snoise(vec2(currentTime * 0.05, currentTime)) * 0.5)) * 8.0;

	float displacementFactor = pow(float(layerIndex) / float(numberOfLayers) * 0.5, 3.0) * layerOffset;
	
	vec3 displacement = (windDirection) * displacementFactor;

	vec3 aNormal = vertexNormal;

	vec3 animated = vec3(vec4(vertexPosition, 1.0)) + aNormal*layerOffset;

	gl_Position = (MVP) * vec4(animated, 1.0);

	gl_Position.y -= 9.82 * displacementFactor;

	gl_Position.x += windDirection.x * displacementFactor;
	gl_Position.y += windDirection.y * displacementFactor;

	UV = uvCoordinate;

	UV3D = vertexPosition * 4.0;

	vertexPositionWorldSpace = vec3(M * vec4(vertexPosition, 1.0));

	vec3 vertexPositionCameraSpace = vec3(V * M * vec4(vertexPosition, 1.0));
	vec3 viewDirectionCameraSpace = vec3(0.0, 0.0, 0.0) - vertexPositionCameraSpace;

	vec3 lightPostionCameraSpace = vec3(V * vec4(lightPosition, 1.0));
	lightDirectionCameraSpace = lightPostionCameraSpace + viewDirectionCameraSpace;

	normal = vec3(transpose(inverse(V * M)) * vec4(vertexNormal, 1.0));
}