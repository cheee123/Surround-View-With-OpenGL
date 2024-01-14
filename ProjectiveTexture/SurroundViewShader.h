#pragma once
#include "ShaderObject.h"
#include <glm/glm.hpp>

class SurroundViewShader: public ShaderObject
{
public:
	SurroundViewShader(std::string vertexShaderPath, std::string fragmentShaderPath);
	~SurroundViewShader();

	bool Init();
	void SetMMat(const glm::mat4& mat);
	void SetMVMat(const glm::mat4& mat);
	void SetPMat(const glm::mat4& mat);
	void SetNormalMat(const glm::mat3& mat);
	void DrawTexture(bool draw);

private:
	void SetCameraMatrix();

	std::string mVertexShaderPath, mFragmentShaderPath;
	int mNumCamera;

	// GLuint mProjTBO, mViewTBO;
	// GLuint mProjTBOTex, mViewTBOTex;

	GLuint mPLoc;
	GLuint mMLoc;
	GLuint mMVLoc;
	GLuint mNormalMatLoc;
	GLuint mDrawTextureLoc;
	// GLuint mProjTBOLoc, mViewTBOLoc
	GLuint mNumCameraLoc;

	GLuint mViewLoc, mProjLoc;
};

