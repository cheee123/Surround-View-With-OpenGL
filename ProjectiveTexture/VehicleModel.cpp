#include "VehicleModel.h"
#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
using namespace std;

VehicleModel::VehicleModel(const std::string& Filename)
{
	scale_x = 1.0;
	scale_y = 1.0;
	scale_z = -0.75;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);
	if (m_pScene) {
		InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}
	indices_size = indices.size();

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);

	glGenBuffers(1, &mVboVertex);
	glBindBuffer(GL_ARRAY_BUFFER, mVboVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

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

VehicleModel::~VehicleModel()
{
	glDeleteBuffers(1, &mVboVertex);
	// glDeleteBuffers(1, &mVboNormal);
	glDeleteBuffers(1, &mEbo);
	glDeleteVertexArrays(1, &mVao);
}

void VehicleModel::Draw()
{
	vehicleTexture.BindTexture();

	glBindVertexArray(mVao);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDrawElements(GL_TRIANGLES, indices_size, GL_UNSIGNED_INT, 0);

	vehicleTexture.UnBindTexture();

   glBindVertexArray(0);
}

bool VehicleModel::InitFromScene(const aiScene* pScene, const string& Filename)
{
	m_Meshes.resize(pScene->mNumMeshes);

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	CountVerticesAndIndices(pScene, NumVertices, NumIndices);

	ReserveSpace(NumVertices, NumIndices);

	InitAllMeshes(pScene);

	if (!InitDiffuses(pScene, Filename)) {
		cout<<"VehicleModel.cpp: cannot load textures!"<<endl;
		return false;
	}

	return true;
}


void VehicleModel::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
{
	for (unsigned int i = 0 ; i < m_Meshes.size() ; i++) {
		m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Meshes[i].BaseVertex = NumVertices;
		m_Meshes[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices  += m_Meshes[i].NumIndices;
	}
}


void VehicleModel::ReserveSpace(unsigned int NumVertices, unsigned int NumIndices)
{
	vertices.reserve(NumVertices*6);
	indices.reserve(NumIndices);
}


void VehicleModel::InitAllMeshes(const aiScene* pScene)
{
	for (unsigned int i = 0 ; i < m_Meshes.size() ; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitSingleMesh(i, paiMesh);
	}
}


void VehicleModel::InitSingleMesh(uint MeshIndex, const aiMesh* paiMesh)
{
	// Populate the vertex attribute vectors
	for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
		const aiVector3D& pPos = paiMesh->mVertices[i];
		vertices.push_back(pPos.x * scale_x);
		vertices.push_back(pPos.y * scale_y);
		vertices.push_back(pPos.z * scale_z);

		const aiVector3D& pTexCoord = paiMesh->mTextureCoords[0][i];
		if(paiMesh->HasTextureCoords(0)) {
			vertices.push_back(pTexCoord.x);
			vertices.push_back(pTexCoord.y);
		}
		else {
			vertices.push_back(0.0);
			vertices.push_back(0.0);
			cout<<"Mesh.cpp: Found a vertex without texcoord."<<endl;
		}

		vertices.push_back(m_Meshes[MeshIndex].MaterialIndex-1);
	}
	
	// Populate the index buffer
	for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		indices.push_back(Face.mIndices[0]+m_Meshes[MeshIndex].BaseVertex);
		indices.push_back(Face.mIndices[1]+m_Meshes[MeshIndex].BaseVertex);
		indices.push_back(Face.mIndices[2]+m_Meshes[MeshIndex].BaseVertex);
		if(Face.mIndices[0]+m_Meshes[MeshIndex].BaseVertex>max_indices)
			max_indices = Face.mIndices[0]+m_Meshes[MeshIndex].BaseVertex;
		if(Face.mIndices[1]+m_Meshes[MeshIndex].BaseVertex>max_indices)
			max_indices = Face.mIndices[1]+m_Meshes[MeshIndex].BaseVertex;
		if(Face.mIndices[2]+m_Meshes[MeshIndex].BaseVertex>max_indices)
			max_indices = Face.mIndices[2]+m_Meshes[MeshIndex].BaseVertex;
	}
}


string VehicleModel::GetDirFromFilename(const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex;
	SlashIndex = Filename.find_last_of("/");

	string Dir;

	if (SlashIndex == string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	return Dir;
}

bool VehicleModel::InitDiffuses(const aiScene* pScene, const string& Filename)
{
	string Dir = GetDirFromFilename(Filename);

	// Initialize the materials
	vector<string> fileList;
	for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		LoadTextures(Dir, pMaterial, i, fileList);
	}
	
	if (!vehicleTexture.LoadTextures(fileList))
	{
		std::cout << "Texture Load Failed" << std::endl;
		return false;
	}

	return true;
}


void VehicleModel::LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index, vector<string>& fileList)
{
	aiString Path;
	if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS){	
		string p(Path.data);

		for (int i = 0 ; i < p.length() ; i++) {
			if (p[i] == '\\') {
				p[i] = '/';
			}
		}

		if (p.substr(0, 2) == ".\\") {
			p = p.substr(2, p.size() - 2);
		}
		string FullPath = Dir + "/" + p;
		fileList.push_back(FullPath);
	}
}

// bool loadOBJ(
// 	const char * path,
//	 std::vector<GLfloat> &vertices,
//	 std::vector<GLuint>  &indices
// ){
//	 float scale_x = 1.0;
//	 float scale_y = 1.0;
//	 float scale_z = 0.75;

// 	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
// 	std::vector<glm::vec3> temp_vertices; 
// 	std::vector<glm::vec2> temp_uvs;
// 	std::vector<glm::vec3> temp_normals;


// 	FILE * file = fopen(path, "r");
// 	if( file == NULL ){
// 		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
// 		getchar();
// 		return false;
// 	}

// 	while( 1 ){

// 		char lineHeader[128];
// 		// read the first word of the line
// 		int res = fscanf(file, "%s", lineHeader);
// 		if (res == EOF)
// 			break; // EOF = End Of File. Quit the loop.

// 		// else : parse lineHeader
		
// 		if ( strcmp( lineHeader, "v" ) == 0 ){
// 			glm::vec3 vertex;
// 			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
// 			temp_vertices.push_back(vertex);
// 		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
// 			glm::vec2 uv;
// 			fscanf(file, "%f %f\n", &uv.x, &uv.y );
// 			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
// 			temp_uvs.push_back(uv);
// 		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
// 			glm::vec3 normal;
// 			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
// 			temp_normals.push_back(normal);
// 		}else if ( strcmp( lineHeader, "f" ) == 0 ){
// 			std::string vertex1, vertex2, vertex3;
// 			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
// 			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
// 			if (matches != 9){
// 				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
// 				fclose(file);
// 				return false;
// 			}
// 			vertexIndices.push_back(vertexIndex[0]);
// 			vertexIndices.push_back(vertexIndex[1]);
// 			vertexIndices.push_back(vertexIndex[2]);
// 			uvIndices	.push_back(uvIndex[0]);
// 			uvIndices	.push_back(uvIndex[1]);
// 			uvIndices	.push_back(uvIndex[2]);
// 			normalIndices.push_back(normalIndex[0]);
// 			normalIndices.push_back(normalIndex[1]);
// 			normalIndices.push_back(normalIndex[2]);

//			 indices.push_back(vertexIndex[0]-1);
//			 indices.push_back(vertexIndex[1]-1);
//			 indices.push_back(vertexIndex[2]-1);
// 		}else{
// 			// Probably a comment, eat up the rest of the line
// 			char stupidBuffer[1000];
// 			fgets(stupidBuffer, 1000, file);
// 		}
// 	}

// 	// For each vertex of each triangle
// 	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

// 		// Get the indices of its attributes
// 		unsigned int vertexIndex = vertexIndices[i];
// 		unsigned int uvIndex = uvIndices[i];
// 		unsigned int normalIndex = normalIndices[i];
		
// 		// Get the attributes thanks to the index
// 		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
// 		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
// 		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
// 		// Put the attributes in buffers
// 		vertices.push_back(vertex[0]*scale_x);
// 		vertices.push_back(vertex[1]*scale_y);
// 		vertices.push_back(vertex[2]*scale_z);
// 		vertices.push_back(uv[0]);
// 		vertices.push_back(uv[1]);
// 	}
// 	fclose(file);
// 	return true;
// }
