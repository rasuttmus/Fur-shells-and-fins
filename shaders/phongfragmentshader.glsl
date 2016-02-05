#version 330 core

uniform mat4  	  NM;
uniform vec3  	  cameraPosition;
uniform vec3  	  color;
uniform vec3  	  ambientColor;
uniform vec3  	  diffuseColor;
uniform vec3  	  specularColor;
uniform vec3  	  lightPosition;
uniform float 	  transparency;
uniform float 	  specularity;
uniform float 	  shinyness;
uniform float 	  lightPower;
uniform sampler2D skinTextureSampler;

in vec3 normal;
in vec3 vertexPositionWorldSpace;
in vec3 lightDirectionCameraSpace;
in vec3 viewDirectionCameraSpace;
in vec2 UV;

out vec4 fragmentColor;

void main() {

	UV;

	float distance = length(lightPosition - vertexPositionWorldSpace);

	vec3 n = normalize(normal);
	vec3 l = normalize(lightDirectionCameraSpace);

	float cosTheta = clamp(dot(n, l), 0, 1);

	vec3 E = normalize(viewDirectionCameraSpace);
	vec3 R = reflect(-l, n);

	float cosAlpha = clamp(dot(E, R), 0, 1);

	vec3 textureColor = texture(skinTextureSampler, UV).rgb;

	fragmentColor.rgb = ambientColor  * textureColor
		  			  + diffuseColor  * textureColor * lightPower * cosTheta
		  			  + shinyness * specularColor * textureColor * lightPower * pow(cosAlpha, specularity);

    //fragmentColor.rgb = texture( textureSampler, UV ).rgb;

	fragmentColor.a = transparency;
}