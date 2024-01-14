#version 300 es
precision highp float;

layout(location = 0) in vec3 vertex;
// layout(location = 1) in vec3 normal;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float cameraID;
layout(location = 3) in float cameraWeight;

uniform mat4 um4mv;
//uniform mat3 um3n;
uniform mat4 um4p;

// out vec3 vNormal;
// out vec3 vViewPos;
out vec2 vTexCoord;
out float vCameraID;
out float vCameraWeight;

void main()
{
	vec4 viewModelPos = um4mv * vec4(vertex, 1.0);
	vTexCoord = texCoord;
	vCameraID = cameraID;
	vCameraWeight = cameraWeight;

	gl_Position = um4p * viewModelPos;
}

// #version 300 es
// precision highp float;

// layout(location = 0) in vec3 vertex;
// layout(location = 1) in vec3 normal;
// layout(location = 2) in vec2 texCoord;
// uniform mat4 um4mv;
// //uniform mat3 um3n;

// uniform mat4 um4p;
// /*
// out VertexData
// {
// 	vec3 vNormal;
// 	vec3 vViewPos;
// 	vec2 vTexCoord;
// 	vec3 vWorldPos;
// } vertexOut;*/

// out vec3 vNormal;
// out vec3 vViewPos;
// out vec2 vTexCoord;
// out vec3 vWorldPos;


// void main()
// {
// 	vec4 viewModelPos = um4mv * vec4(vertex, 1.0);
// 	// vNormal = normalize(um3n * normalize(normal));
// 	vNormal = normalize(transpose(inverse(mat3(um4mv))) * normalize(normal));
// 	vViewPos = viewModelPos.xyz;
// 	vTexCoord = vec2(0.0, 0.0);
// 	vWorldPos = vertex;

// 	gl_Position = um4p * viewModelPos;
// }