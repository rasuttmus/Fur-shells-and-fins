#version 330 core

// Input 
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uvCoordinate;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

out vec3 normal;
out vec3 lightDirectionCameraSpace;
out vec3 viewDirectionCameraSpace;
out vec2 UV;

void main() {

	// Apply MVP matrix to vertex position
	gl_Position = MVP * vec4(vertexPosition, 1.0);

	// Set UV coordinate, which will be passed to the fragment shader
	UV = uvCoordinate;

	// Compute view direction, will be used in the phong shading model
	vec3 vertexPositionCameraSpace = vec3(V * M * vec4(vertexPosition, 1.0));
	viewDirectionCameraSpace = cameraPosition - vertexPositionCameraSpace;

	// Compute light directino, will also be used in the phong model
	vec3 lightPostionCameraSpace = vec3(V * vec4(lightPosition, 1.0));
	lightDirectionCameraSpace = lightPostionCameraSpace + viewDirectionCameraSpace;

	// Transform normal
	normal = vec3(transpose(inverse(V * M)) * vec4(vertexNormal, 1.0));
}