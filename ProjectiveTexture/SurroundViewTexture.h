#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <string>
#include "Common.h"

class SurroundViewTexture
{
public:
	SurroundViewTexture();
	~SurroundViewTexture();

	bool LoadTextures(const std::vector<std::string>& fileList);
	bool UpdateTextures(const std::vector<TextureData>& fileList);
	void BindTexture();
	void UnBindTexture();

private:
	void ReleaseTexture();

	GLuint mTexID;
};

