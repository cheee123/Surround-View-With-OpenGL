#version 300 es
precision highp float;
precision highp sampler2D;
precision highp sampler2DArray;

out vec4 fragColor;

in vec2 vTexCoord;
in float vCameraID;
in float vCameraWeight;

uniform sampler2DArray texImage;

void main(void)
{
	fragColor = vec4(texture(texImage, vec3(vTexCoord, vCameraID)).rgb, vCameraWeight);
	// fragColor = vec4(texture(texImage, vec3(vTexCoord, vCameraID)).rgb * vCameraWeight, 0.0);
	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);

	/*
	// For drawing each camera
	if(vCameraID==0.0)
		fragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else if(vCameraID==1.0)
		fragColor = vec4(0.0, 1.0, 0.0, 1.0);
	else if(vCameraID==2.0)
		fragColor = vec4(0.0, 0.0, 1.0, 1.0);
	else if(vCameraID==3.0)
		fragColor = vec4(1.0, 1.0, 0.0, 1.0);
	else if(vCameraID==4.0)
		fragColor = vec4(1.0, 0.0, 1.0, 1.0);
	else
		fragColor = vec4(0.0, 1.0, 1.0, 1.0);
	*/
}
