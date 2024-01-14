#version 300 es
precision highp float;
precision highp sampler2DArray;

// Outputs colors in RGBA
out vec4 fragColor;

// Imports the texture coordinates from the Vertex Shader
in vec2 vTexCoord;
in float vCameraID;

// Gets the Texture Units from the main function
uniform sampler2DArray texImage;

void main()
{

	//fragColor = vec4(0.5,0.5,0.5,1.0);
	fragColor = vec4(texture(texImage, vec3(0,0, vCameraID)));
}