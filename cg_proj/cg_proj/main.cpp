// Code primarily drawn from following tutorial: https://www.udemy.com/share/1001iYAkYSdVZUQ3Q=/

//esc to quit
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "DirectionalLight.h"

#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Texture checkerTexture;
Texture checkerTexture1;

Model ball;

DirectionalLight mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/vShader.txt";

// Fragment Shader
static const char* fShader = "Shaders/fShader.txt";

//Create floor plane (other objects not used)
void CreatePlane() 
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	Mesh *obj0 = new Mesh();
	obj0->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj0);
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() 
{
	//Initialize window
	mainWindow = Window(800, 600);
	mainWindow.Initialise();

	CreatePlane();
	CreateShaders();

	//Load Textures
	checkerTexture = Texture("Textures/checker.png");
	checkerTexture.LoadTextureA();

	//Import models from folder
	ball = Model();
	ball.LoadModel("Models/ball.obj");

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,  // Color
								0.6f, 0.6f, //ambient intensity
								0.0f, 0.0f, -1.0f); // Direction

	//Default initialize shader uniform variables
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

	//Perspective matrices
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	glm::mat4 orthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
	glm::mat4 orthoView;


	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		//Delta time
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle User Input
		glfwPollEvents();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Shader uniform variables
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		shaderList[0].SetDirectionalLight(&mainLight);

		// Create Viewport
		glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

		//Camera position above scene
		orthoView = glm::lookAt(
			glm::vec3(0.0, 8.0, -1.0), //camera position
			glm::vec3(0.0, 0.0, -1.0), // target
			glm::vec3(0.0, 0.0, -1.0)); //up vector
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(orthoProjection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(orthoView));


		//Floor plane
		glm::mat4 floorModelMat(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floorModelMat));
		checkerTexture.UseTexture();
		meshList[0]->RenderMesh();

		//Ball
		glm::mat4 ballModelMat(1.0f);
		ballModelMat = glm::scale(ballModelMat, glm::vec3(0.065f, 0.065f, 0.065f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ballModelMat));
		ball.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}