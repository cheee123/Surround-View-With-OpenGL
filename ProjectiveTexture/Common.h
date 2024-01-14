#pragma once

#include <GLES3/gl3.h>

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))
#define rad2deg(x) ((180.0f) / ((x)*(3.1415926f)))

typedef struct _TextureData
{
	_TextureData() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} TextureData;

class Common
{
public:

	static void DumpInfo(void);
	static void ShaderLog(GLuint shader);
	static bool CheckShaderCompiled(GLuint shader);
	static bool CheckProgramLinked(GLuint program);
	static bool CheckFrameBufferStatus();
	static bool CheckGLError();
	static void PrintGLError();
	static TextureData Load_png(const char* path, bool mirroredY = true);
	static char** LoadShaderSource(const char* file);
	static void FreeShaderSource(char** srcp);
	
	template<typename T>
	static void UnrealToOpenGLAxis(T& x, T& y, T& z)
	{
		T _x = x, _y = y, _z = z;
		x = _y;
		y = _z;
		z = -_x;
	}

	template<typename T>
	static void UnrealToOpenGLRotation(T& x, T& y, T& z)
	{
		// left hand to right hand
		T _x = x, _y = y, _z = z;
		x = -_y;
		y = _z;
		z = _x;
	}
};
