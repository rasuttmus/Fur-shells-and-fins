#version 330 core

uniform mat4  	  NM;
uniform vec3  	  cameraPosition;
uniform vec3  	  color;
uniform vec3  	  ambientColor;
uniform vec3  	  diffuseColor;
uniform vec3  	  specularColor;
uniform float 	  transparency;
uniform float 	  specularity;
uniform float 	  shinyness;
uniform float 	  lightPower;
uniform sampler2D skinTextureSampler;

in vec3 normal;
in vec3 lightDirectionCameraSpace;
in vec3 viewDirectionCameraSpace;
in vec2 UV;

out vec4 fragmentColor;

void main() {

	// Compute vectors and angle for the shading
	vec3 n         = normalize(normal);
	vec3 l 		   = normalize(lightDirectionCameraSpace);
	float cosTheta = clamp(dot(n, l), 0, 1);
	vec3 E 		   = normalize(viewDirectionCameraSpace);
	vec3 R 		   = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

	// Get color sample from our texture, this is the skin color
	vec3 textureColor = texture(skinTextureSampler, UV).rgb;

	// Apply shading and color to our fragment
	fragmentColor.rgb = ambientColor  * textureColor
		  			  + diffuseColor  * textureColor * lightPower * cosTheta
		  			  + shinyness * specularColor * textureColor * lightPower * pow(cosAlpha, specularity);

	// Just set the alpha accorign to the GUI setting
	fragmentColor.a = transparency;
}