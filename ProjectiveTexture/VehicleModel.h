#pragma once

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <array>
#include <cstring>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals |  aiProcess_JoinIdenticalVertices )
#define INVALID_MATERIAL 0xFFFFFFFF
using namespace std;

#include "SurroundViewTexture.h"
#include "Common.h"


class VehicleModel
{
public:

	VehicleModel(const std::string& Filename);
	~VehicleModel();
	void Draw();

	GLuint mVao, mVboVertex, mVboNormal, mEbo;
	std::vector<GLfloat> vertices;
	std::vector<GLuint>  indices;
	int indices_size;
	int max_indices=0;

	SurroundViewTexture vehicleTexture;

	float scale_x, scale_y, scale_z;

    bool LoadMesh(const std::string& Filename);

protected:

    virtual void ReserveSpace(uint NumVertices, uint NumIndices);

    virtual void InitSingleMesh(uint MeshIndex, const aiMesh* paiMesh);

    struct BasicMeshEntry {
        BasicMeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        uint NumIndices;
        uint BaseVertex;
        uint BaseIndex;
        uint MaterialIndex;
    };

    std::vector<BasicMeshEntry> m_Meshes;

    const aiScene* m_pScene;

private:

    bool InitFromScene(const aiScene* pScene, const std::string& Filename);

    void CountVerticesAndIndices(const aiScene* pScene, uint& NumVertices, uint& NumIndices);

    void InitAllMeshes(const aiScene* pScene);

    bool InitDiffuses(const aiScene* pScene, const std::string& Filename);

    void LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index, vector<string>& fileList);

    std::string GetDirFromFilename(const string& Filename);

    enum BUFFER_TYPE {
        INDEX_BUFFER = 0,
        POS_VB       = 1,
        TEXCOORD_VB  = 2,
        NUM_BUFFERS  = 3
    };

    Assimp::Importer m_Importer;
};

