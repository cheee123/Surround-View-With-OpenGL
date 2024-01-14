#define GLFW_INCLUDE_ES3
#include "preparation.txt"
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include <set>

#include "SurroundViewShader.h"
#include "SurroundViewTexture.h"
#include "BowlModel.h"
#include "VehicleModel.h"
#include "Camera.h"

using framerate = std::chrono::duration<int, std::ratio<1, 30>>;
int windowWidth = 640;
int windowHeight = 640;

float fov = 90.0f;
float nearClip = 0.1f;
float farClip = 1000.0f;
float rangeOffsetX = 0.0;
float rangeOffsetZ = 40.0;
float baseRange = 20.0;
glm::vec3 baseSize = glm::vec3(2.5f, 10.990f, 3.402f);

GLFWwindow* window;
SurroundViewShader* surroundViewShader = nullptr;
SurroundViewShader* vehicleShader = nullptr;

SurroundViewTexture surroundViewTexture;
BowlModel* bowlModel = nullptr;
VehicleModel* vehicleModel = nullptr;
Camera camera;
std::vector<std::vector<std::string>> sourceImages;
std::queue<std::vector<TextureData>> sourceImageData;
std::mutex surroundTextureLock;

std::vector<std::vector<std::string>> getFileList(std::string rootDir)
{
	std::vector<std::vector<std::string>> fileList;
	bool new_frame = true;
	for(int i : chosen_cam){	
		std::set<std::string> files_s;
		
		for (const auto& fileName : std::filesystem::recursive_directory_iterator(FILEPATH+std::to_string(i)))
		{
			if ((fileName.path().extension().string() == ".jpg" || fileName.path().extension().string() == ".png"))
			{
				files_s.insert(fileName.path().string());
			}
		}
		if(new_frame){
			new_frame = false;
			for (const auto& fileName : files_s)
			{
				std::vector<std::string> files;
				files.emplace_back(fileName);
				fileList.emplace_back(files);
			}
		}
		else{
			int frame = 0;
			for (const auto& fileName : files_s)
			{
				fileList[frame++].emplace_back(fileName);
			}
		}
	}
	return fileList;
}
/*
std::vector<std::vector<std::string>> getFileList(std::string rootDir)
{
	std::vector<std::vector<std::string>> fileList;

	std::set<std::string> subDir_s;
	for (auto &entry : std::filesystem::directory_iterator(rootDir))
	{
		subDir_s.insert(entry.path());
	}

	int count = 0;

	for (auto &dirName : subDir_s)
	{
		std::vector<std::string> files;

		std::set<std::string> files_s;
		
		for (const auto& fileName : std::filesystem::recursive_directory_iterator(dirName))
		{
			if ((fileName.path().extension().string() == ".jpg" || fileName.path().extension().string() == ".png") && count < 21)
			{
				files_s.insert(fileName.path().string());
			}
			
		}

		for (const auto& fileName : files_s)
		{
			files.emplace_back(fileName);
			// std::cout << fileName << std::endl;
		}

		fileList.emplace_back(files);
		++count;
	
	}
	return fileList;
}
*/
void Init()
{
	surroundViewShader = new SurroundViewShader("./resources/drawModel.vs.glsl", "./resources/drawModel.fs.glsl");
	vehicleShader = new SurroundViewShader("./resources/vehicle.vs.glsl", "./resources/vehicle.fs.glsl");

	surroundViewShader->Init();
	vehicleShader->Init();

	sourceImages = getFileList(FILEPATH);
	
	if (!surroundViewTexture.LoadTextures(sourceImages[0]))
	{
		std::cout << "Texture Load Failed" << std::endl;
	}
	
	bowlModel = new BowlModel();
	vehicleModel = new VehicleModel("./resources/models/vehicle/vehicle.obj");
	glm::vec3 cameraPos( 0, 3.402, 0);
	glm::vec3 viewDir(0, 0, -1);
	viewDir = glm::normalize(viewDir);
	glm::vec3 upDir(0, 1, 0);
	camera.SetViewport(0, 0, windowWidth, windowHeight);
	camera.SetFOV(glm::radians(90.0f));
	camera.SetPosition(cameraPos);
	camera.SetLookAt(cameraPos + viewDir, upDir);
	camera.SetClipping(nearClip, farClip);
	camera.SetMode(CameraType::FREE);

	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	/*glCullFace(GL_FRONT_AND_BACK);
	glFrontFace(GL_CCW);*/
}

void RenderScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.Update();

	glm::mat4 _projMat, _viewMat, _modelMat;
	camera.GetMatricies(_projMat, _viewMat, _modelMat);

	glm::mat4 mvMat = _viewMat * _modelMat;
	// std::cout << glm::to_string(_modelMat) << "\n";
	
	surroundViewShader->Enable();
	surroundViewShader->DrawTexture(true);
	surroundViewShader->SetMVMat(mvMat);
	// surroundViewShader->SetMMat(_modelMat);
	surroundViewShader->SetPMat(_projMat);
	// surroundViewShader->SetNormalMat(normalMat);
	surroundViewTexture.BindTexture();
	bowlModel->Draw();
	surroundViewTexture.UnBindTexture();
	surroundViewShader->Disable();

	vehicleShader->Enable();
	vehicleShader->DrawTexture(true);
	vehicleShader->SetMVMat(mvMat);
	// vehicleShader->SetMMat(_modelMat);
	vehicleShader->SetPMat(_projMat);
	// vehicleShader->SetNormalMat(normalMat);
	vehicleModel->Draw();
	vehicleShader->Disable();
}

void WindowSizeChangeCB(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	camera.SetViewport(0, 0, windowWidth, windowHeight);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_W:
		camera.Move(CameraDirection::FORWARD);
		break;
	case GLFW_KEY_A:
		camera.Move(CameraDirection::LEFT);
		break;
	case GLFW_KEY_S:
		camera.Move(CameraDirection::BACK);
		break;
	case GLFW_KEY_D:
		camera.Move(CameraDirection::RIGHT);
		break;
	case GLFW_KEY_Q:
		camera.Move(CameraDirection::DOWN);
		break;
	case GLFW_KEY_E:
		camera.Move(CameraDirection::UP);
		break;
	default:
		break;
	}
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.Move((yoffset > 0) ? CameraDirection::FORWARD : CameraDirection::BACK);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	camera.SetPos(button, action, xpos, ypos);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	camera.Move2D(xpos, ypos);
}

void UpdateTexture()
{
	size_t startFrame = 1;
	while (startFrame < sourceImages.size())
	{
		while (sourceImageData.size() < 1)
		{
			std::vector<TextureData> imageData;
			for (int i = 0; i < sourceImages[startFrame].size(); ++i)
			{
				TextureData tdata = Common::Load_png(sourceImages[startFrame][i].c_str());
				imageData.push_back(tdata);
				//std::cout << startFrame << " " << sourceImages[startFrame][i] << std::endl;
			}
			std::lock_guard lock(surroundTextureLock);
			sourceImageData.push(imageData);
			++startFrame;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


int main(int argc, char* argv[])
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

	window = glfwCreateWindow(windowWidth, windowHeight, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, WindowSizeChangeCB);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	Init();

	auto curTime = std::chrono::system_clock::now();
	std::this_thread::sleep_until(curTime + framerate{ 150 });
	std::thread updateThread(UpdateTexture);

	curTime = std::chrono::system_clock::now();
	auto nextTime = curTime + framerate{ 1 };

	// while (true)
	// {
	// 	if (sourceImageData.size() > 0){
	// 		const std::vector<TextureData>& tdata = sourceImageData.front();
	// 		surroundViewTexture.UpdateTextures(tdata);
	// 		break;
	// 	}
	// }
	// updateThread.detach();

	// Calculate FPS
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	while (!glfwWindowShouldClose(window))
	{
		// std::this_thread::sleep_until(nextTime);
		// nextTime += framerate{ 1 };
		
		glfwPollEvents();
		RenderScene();
		//RenderGUI();
		glfwSwapBuffers(window);

		// Calculate FPS
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;
		if (timeDiff >= 2.0){
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			//std::string newTitle = "Simple example - " + FPS + "FPS / " + ms + "ms";
			//glfwSetWindowTitle(window, newTitle.c_str());
			fflush(stdout);
			std::cout << "\r" + FPS + "FPS / " + ms + "ms ";

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}
		// if (sourceImageData.size() > 0)
		// {
		// 	std::lock_guard lock(surroundTextureLock);
		// 	//std::cout << sourceImageData.size() << std::endl;
		// 	const std::vector<TextureData>& tdata = sourceImageData.front();
		// 	surroundViewTexture.UpdateTextures(tdata);
		// 	for (int i = 0; i < tdata.size(); ++i)
		// 	{
		// 		delete[] tdata[i].data;
		// 	}
		// 	sourceImageData.pop();
		// }
	}
	std::cout << "\n";
	// updateThread.join();

	if (surroundViewShader)
	{
		delete surroundViewShader;
	}
	if (vehicleShader)
	{
		delete vehicleShader;
	}
	if (bowlModel)
	{
		delete bowlModel;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}
