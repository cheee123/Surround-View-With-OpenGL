#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <iostream>
#include <fstream>

class BowlModel
{
public:

	BowlModel();
	~BowlModel();
	void Draw();

private:
	GLuint mVao, mVboVertex, mVboNormal, mEbo;
	std::vector<GLfloat> vertices;
	std::vector<GLuint>  indices;
	int indices_size;
};

