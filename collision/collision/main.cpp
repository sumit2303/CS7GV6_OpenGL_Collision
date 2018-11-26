// Structure primarily drawn from following tutorial: https://www.udemy.com/share/1001iYAkYSdVZUQ3Q=/

//esc to quit
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h> //abs

#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>

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
#include "BallObject.h"
#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
//window size
GLuint Width = 800;
GLuint Height = 800;

std::vector<Shader> shaderList;
std::vector<BallObject*> ballList;
std::vector <glm::mat4> modelMatrices;

Texture greenTexture;

Model ball;

DirectionalLight mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat rotationStep = 100.0f;
GLfloat maxRotate = 360.0f;
GLboolean collisionTest;

// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

//initial ball position
glm::vec2 ballPosition(0.0f, 0.0f);

//For translation
GLfloat curPositionX = 0.0f;
GLfloat curPositionZ = 0.0f;
GLfloat maxTranslate = 9.0f;
GLfloat minTranslate = -9.0f;

// Vertex Shader
static const char* vShader = "Shaders/vShader.txt";

// Fragment Shader
static const char* fShader = "Shaders/fShader.txt";

//Create floor plane (other objects not used)
Mesh CreatePlane()
{
	unsigned int Indices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat Vertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,			0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,		0.0f, -1.0f, 0.0f
	};
	Mesh *floor = new Mesh();
	floor->CreateMesh(Vertices, Indices, 32, 6);
	return *floor;
}


void TranslateModels(bool* keys) {
	if (keys[GLFW_KEY_SPACE])

	{
		if (!ballList.empty()) {
			ballList[0]->Velocity.x = 10.0f * deltaTime;
			ballList[0]->Velocity.y = 10.0f * deltaTime;
			//ballList[0]->rotationOn = true;
			invertRot = !invertRot;
			/*for (int i = 0; i < ballList.size(); i++) {
				ballList[i]->Velocity.x = 10.0f * deltaTime;
				ballList[i]->Velocity.y = 10.0f * deltaTime;
			}*/
		}
	}

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


GLchar CheckCollisionAABB(BallObject &one, BallObject &two)
{
	//check difference between the balls'centers
	//glm::vec2 difference = one.Position - two.Position;
	glm::vec2 difference = glm::vec2(std::abs(one.Position.x - two.Position.x), std::abs(one.Position.y - two.Position.y));
	//if distance between centers <= 2*radius --> collision
	if (glm::length(difference) <= (2 * one.Radius)) {
		return GL_TRUE;
	}
	else
		// No collision, return tuple with collision as false
		return GL_FALSE;
}

void DoCollisions(BallObject *ball1, BallObject *ball2)
{
	//check for collisions
	GLchar collisionTest = CheckCollisionAABB(*ball1, *ball2);
	if (collisionTest)
	{
		//collision resolution
		glm::vec2 normalBall1 = glm::vec2(ball1->Position.x - ball2->Position.x, ball1->Position.y - ball2->Position.y);
		glm::vec2 newVelocity = glm::vec2(ball1->Velocity.x + normalBall1.x, ball1->Velocity.y + normalBall1.y);
		ball1->Velocity = newVelocity;
	
		glm::vec2 normalBall2 = glm::vec2(ball2->Position.x - ball1->Position.x, ball2->Position.y - ball1->Position.y);
		newVelocity = glm::vec2(ball2->Velocity.x + normalBall2.x, ball2->Velocity.y + normalBall2.y);
		ball2->Velocity = newVelocity;

		//decrease velocity
		ball1->Velocity /= 10.0f;
		ball2->Velocity /= 10.0f;
		ball1->rotationOn = true;
		ball2->rotationOn = true;
	}
}


void Update(GLfloat dt, GLuint uniformModel)
{
	if (!ballList.empty()) {
		//Check for collision with walls for each ball
		for (int i = 0; i < ballList.size(); i++) {
			ballList[i]->Move(dt, Width);
			//Check for collision between balls
			for (int j = i + 1; j < ballList.size(); j++) {
				DoCollisions(ballList[i], ballList[j]);
			}
			// Handle rotation for each ball
			if (ballList[i]->rotationOn == true) {
				//if (invertRot == false) {
				ballList[i]->Rotation += rotationStep * deltaTime;
				if (ballList[i]->Rotation > maxRotate) {
					ballList[i]->Rotation = 0.0f;
				}
			}
		}
	}
}



int main()
{
	//Initialize window
	mainWindow = Window(Width, Height);
	mainWindow.Initialise();

	Mesh floorMesh = CreatePlane();
	CreateShaders();

	//Load Textures
	greenTexture = Texture("Textures/green.png");
	greenTexture.LoadTextureA();

	//Import models from folder
	ball = Model();
	ball.LoadModel("Models/ball.obj");

	mainLight = DirectionalLight(
		1.0f, 1.0f, 1.0f,  // Color
		0.6f, 0.6f, //ambient intensity
		0.0f, 0.0f, -1.0f); // Direction

//Default initialize shader uniform variables
	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;

	//Perspective matrices
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
		//Check for input transformations
		TranslateModels(mainWindow.getsKeys());
		mainWindow.getXChange();
		mainWindow.getYChange();

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
			glm::vec3(0.0, 50.0, 0.0), //camera position
			glm::vec3(0.0, 0.0, 0.0), // target
			glm::vec3(0.0, 0.0, -1.0)); //up vector
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(orthoProjection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(orthoView));

		//Render Floor (Plane)
		glm::mat4 floorModelMat(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floorModelMat));
		greenTexture.UseTexture();
		floorMesh.RenderMesh();

		// Handle mouse clicks, need to convert points from screen space to world space
		float aspect = 800.0f / Height;
		glm::vec4 viewPort = glm::vec4(0.0f, 0.0f, 800.0f, 800.0f);
		glm::mat4 projectionMatrix = orthoProjection;
		glm::mat4 modelWorldMatrix = orthoView;

		glm::vec3 screenPoint1 = glm::vec3(mainWindow.lastX, mainWindow.lastY, 1.0f);
		//Unproject both these points
		glm::vec3 modelPoint1 = unProject(screenPoint1, orthoView, orthoProjection, viewPort);
		// Since ortho and looking down, will leave y == 1
		// OpenGL and Windows have different points for 0,0, so multiple Z (y-axis in this case) by -1 to correct it
		modelPoint1.y = 1.0f;
		modelPoint1.z *= -1.0f;

		//Ball - spawn
		if (mainWindow.mousePressed) {
			//std::cout << "[WORLD COORDS]mouse at: " << modelPoint1.x << ", " << modelPoint1.y << ", " << modelPoint1.z << ", " << std::endl;
			BallObject* addBall = new BallObject();
			addBall->Position = glm::vec2(modelPoint1.x, modelPoint1.z);
			ballList.push_back(addBall);
			
			// Need to turn off mouse pressed here because by the time it comes back to here on the next loop...
			// ...it will have logged 3 clicks
			mainWindow.mousePressed = !mainWindow.mousePressed;
		}
		

		//Check for collision
		Update(deltaTime, uniformModel);
		
		// Render each ball in vector of ball objects
		for (int i = 0; i < ballList.size(); i++)
			ballList[i]->Draw(ball, uniformModel);

		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}