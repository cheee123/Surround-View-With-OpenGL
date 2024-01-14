#include "SurroundViewTexture.h"

SurroundViewTexture::SurroundViewTexture()
{
	mTexID = -1;
}

SurroundViewTexture::~SurroundViewTexture()
{
	ReleaseTexture();
}

bool SurroundViewTexture::LoadTextures(const std::vector<std::string>& fileList)
{
	ReleaseTexture();

	for (int i = 0; i < fileList.size(); ++i)
	{
		TextureData tdata = Common::Load_png(fileList[i].c_str());
		if (tdata.data == nullptr)
		{
			return false;
		}

		if (i == 0)
		{
			glGenTextures(1, &mTexID);
			glBindTexture(GL_TEXTURE_2D_ARRAY, mTexID);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tdata.width, tdata.height, fileList.size());

			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glPixelStorei(GL_UNPACK_ROW_LENGTH, tdata.width);
		}
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tdata.width, tdata.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
		delete[] tdata.data;
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return true;
}

bool SurroundViewTexture::UpdateTextures(const std::vector<TextureData>& fileList)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, mTexID);
	for (int i = 0; i < fileList.size(); ++i)
	{
		TextureData tdata = fileList[i];
		if (tdata.data == nullptr)
		{
			return false;
		}
		
		glPixelStorei(GL_UNPACK_ROW_LENGTH, tdata.width);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tdata.width, tdata.height, 1, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
	}

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return true;
}

void SurroundViewTexture::BindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, mTexID);
}

void SurroundViewTexture::UnBindTexture()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void SurroundViewTexture::ReleaseTexture()
{
	glDeleteTextures(1, &mTexID);
}
