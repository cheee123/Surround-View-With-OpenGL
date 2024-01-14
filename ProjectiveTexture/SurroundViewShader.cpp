#include "SurroundViewShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include "Common.h"

SurroundViewShader::SurroundViewShader(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	mVertexShaderPath = vertexShaderPath;
	mFragmentShaderPath = fragmentShaderPath;
}


SurroundViewShader::~SurroundViewShader()
{
	// todo release ununsed resource
}

bool SurroundViewShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, mVertexShaderPath))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, mFragmentShaderPath))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	mMVLoc = GetUniformLocation("um4mv");
	if (mMVLoc == -1)
	{
		puts("Get uniform location error: um4mv");
	}

	// mMLoc = GetUniformLocation("modelMat");
	// if (mMLoc == -1)
	// {
	// 	puts("Get uniform location error: modelMat");
	// }

	mPLoc = GetUniformLocation("um4p");
	if (mPLoc == -1)
	{
		puts("Get uniform location error: um4p");
	}

	// mNormalMatLoc = GetUniformLocation("um3n");
	// if (mNormalMatLoc == -1)
	// {
	// 	puts("Get uniform location error: um3n");
	// }

	// mDrawTextureLoc = GetUniformLocation("drawTexture");
	// if (mDrawTextureLoc == -1)
	// {
	// 	puts("Get uniform location error: drawTexture");
	// }

	// mProjTBOLoc = GetUniformLocation("projTBO");
	// if (mProjTBOLoc == -1)
	// {
	// 	puts("Get uniform location error: projTBO");
	// }

	// mViewTBOLoc = GetUniformLocation("viewTBO");
	// if (mViewTBOLoc == -1)
	// {
	// 	puts("Get uniform location error: viewTBO");
	// }

	// mNumCameraLoc = GetUniformLocation("numCamera");
	// if (mNumCameraLoc == -1)
	// {
	// 	puts("Get uniform location error: numCamera");
	// }

	// SetCameraMatrix();
	//float A = -100 + 0.1;
	//float B = 100 * 0.1;
	//glm::mat4 _K = glm::mat4(
	//	-1090.130459130525, -0.2459070705342173, -(1280 - 639.2479606676703), 0,
	//	0, -1089.835007211895, -(1280 - 638.5019874506537), 0,
	//	0, 0, A, B,
	//	0, 0, 1, 0);
	//glm::mat4 NDC = glm::ortho(-640.0f, 640.0f, -640.0f, 640.0f);
	//_K = NDC * _K;

	return true;
}

void SurroundViewShader::SetMMat(const glm::mat4& mat)
{
	// glUniformMatrix4fv(mMLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void SurroundViewShader::SetMVMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(mMVLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void SurroundViewShader::SetPMat(const glm::mat4& mat)
{
	glUniformMatrix4fv(mPLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void SurroundViewShader::SetNormalMat(const glm::mat3& mat)
{
	// glUniformMatrix3fv(mNormalMatLoc, 1, GL_FALSE, glm::value_ptr(mat));
	// std::cout<< glm::to_string(mat) << std::endl;
}

void SurroundViewShader::DrawTexture(bool draw)
{
	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_BUFFER, mProjTBOTex);
	// glUniform1i(mProjTBOLoc, 1);

	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_BUFFER, mViewTBOTex);
	// glUniform1i(mViewTBOLoc, 2);

	// glUniform1i(mNumCameraLoc, mNumCamera);
	// glUniform1i(mDrawTextureLoc, draw);
}

void SurroundViewShader::SetCameraMatrix()
{
	
}
