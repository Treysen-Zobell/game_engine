
// OpenGL
#include <glad\glad.h>
#include <glfw\glfw3.h>

// Standard
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <map>

// Include Libraries
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <openAL/al.h>
#include <openAL/alc.h>

#include "stb_image.h"

// Headers
#include "TessellationShader.h"
#include "FrameBufferObject.h"
#include "ReflectionShader.h"
#include "OpenALFunctions.h"
#include "OpenGLFunctions.h"
#include "DisplayManager.h"
#include "StaticShader.h"
#include "NormalShader.h"
#include "SkyboxShader.h"
#include "SkyboxModel.h"
#include "AssetLoader.h"
#include "FPSShader.h"
#include "FpsModel.h"
#include "Listener.h"
#include "Texture.h"
#include "Source.h"
#include "Vertex.h"
#include "Shader.h"
#include "Config.h"
#include "Camera.h"
#include "Debug.h"
#include "Maths.h"
#include "Model.h"
#include "Light.h"
#include "Mesh.h"

int main() {
	std::chrono::time_point loadTimeStart = std::chrono::high_resolution_clock::now();

	Listener listener = Listener();

	Source source1 = Source("res/audio/ambientMono.wav", glm::vec3(-4.25f, 0.125f, -4.25f), glm::vec3(0.0f), 1.0f, 1.0f, 0.0f, 10.0f, 1.0f, AL_FALSE);
	Source source2 = Source("res/audio/heavy.wav");

	DisplayManager::createDisplay(Config::Display::WIDTH, Config::Display::HEIGHT);

	Shader shader = Shader(
		"shaders/shader/shader.vert",
		"shaders/shader/shader.frag");

	StaticShader staticShader = StaticShader(
		"shaders/staticShader/staticShader.vert", 
		"shaders/staticShader/staticShader.frag");

	NormalShader normalShader = NormalShader(
		"shaders/normalShader/normalShader.vert",
		"shaders/normalShader/normalShader.frag");

	TessellationShader tessShader = TessellationShader(
		"shaders/tessellationShader/vertex.vert",
		"shaders/tessellationShader/fragment.frag",
		"shaders/tessellationShader/control.tesc",
		"shaders/tessellationShader/evaluation.tese");
		// "shaders/tessellationShader/geometry.geom");

	ReflectionShader reflectionShader = ReflectionShader(
		"shaders/reflectionShader/shader.vert",
		"shaders/reflectionShader/shader.frag");

	SkyboxShader skyboxShader = SkyboxShader(
		"shaders/skyboxShader/shader.vert",
		"shaders/skyboxShader/shader.frag");

	FPSShader fpsShader = FPSShader(
		"shaders/fpsShader/vertexShaderFPS.vert",
		"shaders/fpsShader/fragmentShaderFPS.frag");

	Light light(glm::vec3(60.0f, 100.0f, 100.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	FpsModel fpsModel = FpsModel();
	SkyboxModel skyboxModel = SkyboxModel("res/skyboxDay");
	std::vector<Model> assimpModels = AssetLoader::loadModels("res/models.scene");

	DisplayManager::hideCursor();
	//DisplayManager::showCursor();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::chrono::time_point loadTimeEnd = std::chrono::high_resolution_clock::now();
	std::chrono::microseconds executionTime = std::chrono::duration_cast<std::chrono::microseconds>(loadTimeEnd - loadTimeStart);
	std::cout << "Execution Time: " << executionTime.count() << std::endl;

	FrameBufferObject fbo = FrameBufferObject();

	source1.play();

	while (!glfwWindowShouldClose(DisplayManager::window) && !glfwGetKey(DisplayManager::window, GLFW_KEY_ESCAPE)) {

		if (glfwGetKey(DisplayManager::window, GLFW_KEY_0))
			source2.play();
		source2.setPosition(Camera::position);

		Camera::move();

		listener.updatePosition();


		// Buffered Shader Cycle
		fbo.bind();
		shader.start();
		for (Model assimpModel : assimpModels)
			assimpModel.draw(shader);
		shader.stop();
		fbo.unbind();

		// Clear Screen Buffers
		glCall(glClearColor, 0.0f, 1.0f, 1.0f, 1.0f);
		glCall(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Shader Cycle
		GLuint tempTexID = assimpModels[0].meshes[0].textures[0].id;
		assimpModels[0].meshes[0].textures[0].id = fbo.textureColorID;
		shader.start();
		for (Model assimpModel : assimpModels)
			assimpModel.draw(shader);
		shader.stop();
		assimpModels[0].meshes[0].textures[0].id = tempTexID;

		// Static Shader Cycle
		staticShader.start();
		for (Model assimpModel : assimpModels)
		{
			assimpModel.increasePosition(glm::vec3(-10.0f, 0.0f, 0.0f));
			assimpModel.draw(staticShader, light);
			assimpModel.increasePosition(glm::vec3(10.0f, 0.0f, 0.0f));
		}
		staticShader.stop();

		// Normal Shader Cycle
		normalShader.start();
		for (Model assimpModel : assimpModels)
		{
			assimpModel.increasePosition(glm::vec3(-20.0f, 0.0f, 0.0f));
			assimpModel.draw(normalShader, light);
			assimpModel.increasePosition(glm::vec3(20.0f, 0.0f, 0.0f));
		}
		normalShader.stop();

		// Tessellation Shader Cycle
		tessShader.start();
		for (Model assimpModel : assimpModels)
		{
			assimpModel.increasePosition(glm::vec3(-30.0f, 0.0f, 0.0f));
			assimpModel.draw(tessShader, light);
			assimpModel.increasePosition(glm::vec3(30.0f, 0.0f, 0.0f));
		}
		tessShader.stop();

		// Reflection Shader Cycle
		reflectionShader.start();
		for (Model assimpModel : assimpModels)
		{
			assimpModel.increasePosition(glm::vec3(-40.0f, 0.0f, 0.0f));
			assimpModel.draw(reflectionShader, light);
			assimpModel.increasePosition(glm::vec3(40.0f, 0.0f, 0.0f));
		}
		reflectionShader.stop();

		// Skybox Shader Cycle
		skyboxShader.start();
		skyboxModel.draw(skyboxShader);
		skyboxShader.stop();

		// FPS Shader Cycle
		fpsModel.update();
		fpsModel.render(fpsShader);

		// Show Display Buffer
		DisplayManager::updateDisplay();
	}

	shader.cleanUp();
	staticShader.cleanUp();
	normalShader.cleanUp();
	tessShader.cleanUp();
	fpsShader.cleanUp();
	AssetLoader::cleanUp();
	DisplayManager::closeDisplay();
}
