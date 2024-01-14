#version 300 es
precision highp float;

// Positions/Coordinates
layout (location = 0) in vec3 vertex;
// Texture Coordinates
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float CameraID;

out vec2 vTexCoord;
out float vCameraID;

// Imports the camera matrix
uniform mat4 um4mv;
uniform mat4 um4p;

void main()
{
	
	vec4 viewModelPos = um4mv * vec4(vertex, 1.0);
	vTexCoord = texCoord;
	vCameraID = CameraID;
	gl_Position = um4p * viewModelPos;
}
