#include "BowlModel.h"
#include "Common.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

BowlModel::BowlModel()
{
	std::ifstream inputFile;
	
	// Read vertices
	inputFile.open("./resources/Bowl_vertices.txt");
	if(inputFile.is_open())
	{
		GLfloat vertex;
		while(inputFile >> vertex)
			vertices.push_back(vertex); // Add values to the vector
		
		inputFile.close();              // Close the input file
	}
	else
		std::cout << "Unable to open the file: " << "./resources/Bowl_vertices.txt" << std::endl;
	
	// Read indices
	inputFile.open("./resources/Bowl_indices.txt");
	if (inputFile.is_open())
	{
		GLuint index;
		while(inputFile >> index)
			indices.push_back(index); // Add values to the vector
		
		inputFile.close();            // Close the input file        
        indices_size = indices.size();
	}
	else
		std::cout << "Unable to open the file: " << "./resources/Bowl_indices.txt" << std::endl;
	

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);

	glGenBuffers(1, &mVboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, mVboVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// glGenBuffers(1, &mVboNormal);
	// glBindBuffer(GL_ARRAY_BUFFER, mVboNormal);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), normals.data(), GL_STATIC_DRAW);
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// glEnableVertexAttribArray(1);

	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

BowlModel::~BowlModel()
{
	glDeleteBuffers(1, &mVboVertex);
	// glDeleteBuffers(1, &mVboNormal);
	glDeleteBuffers(1, &mEbo);
	glDeleteVertexArrays(1, &mVao);
}

void BowlModel::Draw()
{
	glBindVertexArray(mVao);
	glDisable(GL_DEPTH_TEST);
	// glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendFunc(GL_ONE, GL_ONE);
	// glBlendFunc(GL_ONE_MINUS_DST_ALPHA , GL_DST_ALPHA);
	// glBlendFunc(GL_SRC_ALPHA , GL_DST_ALPHA);
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_DST_ALPHA, GL_SRC_ALPHA, GL_DST_ALPHA);
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ONE);
	// glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ONE);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ZERO, GL_SRC_ALPHA, GL_ZERO);

	glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);
	
	// For drawing grids
	// glDrawElements(GL_LINES, indices_size*4, GL_UNSIGNED_INT, NULL);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}
