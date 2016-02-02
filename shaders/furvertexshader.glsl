#version 330 core

// Input 
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uvCoordinate;
layout(location = 2) in vec3 vertexNormal;

uniform mat4  MVP;
uniform mat4  MV;
uniform mat4  M;
uniform mat4  V;
uniform mat4  MVLight;
uniform vec3  lightPosition;
uniform float layerOffset;

out vec3 normal;
out vec3 vertexPositionWorldSpace;
out vec3 vertexPositionModelSpace;
out vec3 lightDirectionCameraSpace;
out vec2 UV;

void main() {

	vertexPositionModelSpace = vertexPosition;

	gl_Position = MVP * vec4(vertexPosition + vertexNormal * layerOffset, 1.0);

	gl_Position.y -= layerOffset * layerOffset * 5.0;

	UV = uvCoordinate;

	vertexPositionWorldSpace = vec3(M * vec4(vertexPosition, 1.0));

	vec3 vertexPositionCameraSpace = vec3(V * M * vec4(vertexPosition, 1.0));
	vec3 viewDirectionCameraSpace = vec3(0.0, 0.0, 0.0) - vertexPositionCameraSpace;

	vec3 lightPostionCameraSpace = vec3(V * vec4(lightPosition, 1.0));
	lightDirectionCameraSpace = lightPostionCameraSpace + viewDirectionCameraSpace;

	normal = vec3(transpose(inverse(V * M)) * vec4(vertexNormal, 1.0));
}