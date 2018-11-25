// Code primarily drawn from following tutorial: https://www.udemy.com/share/1001iYAkYSdVZUQ3Q=/

//esc to quit
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h> //abs

#include <iostream>
#include <tuple>
#include <functional>

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
#include "BallObject.h"

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

const float toRadians = 3.14159265f / 180.0f;
const GLfloat BALL_RADIUS = 1.0f;

Window mainWindow;
//window size
GLuint Width = 800;
GLuint Height = 800;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Texture greenTexture;
Texture redTexture;
Texture whiteTexture;
Texture blueTexture;

//Model disk;
Model ball;

//ball object
BallObject *ball1, *ball2; 
GLboolean collisionTest;
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
//initial ball position
glm::vec2 ballPosition(0.0f, 0.0f);

DirectionalLight mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
void CreatePlane()
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
	Mesh *obj0 = new Mesh();
	obj0->CreateMesh(Vertices, Indices, 32, 6);
	meshList.push_back(obj0);
}

void TranslateModels(bool* keys) {
	if (keys[GLFW_KEY_RIGHT])
	{
		if (ball1->Position.x <= maxTranslate) {
			ball1->Position.x += 0.1f;
		}
		else {
			ball1->Position.x= ball1->Position.x;
		}
	}

	if (keys[GLFW_KEY_LEFT])
	{
		if (ball1->Position.x >= minTranslate) {
			ball1->Position.x =(ball1->Position.x* ball1->Velocity.x)+0.1f;
			//ball1->Position.x -= ball1->Velocity.x+0.1f;
		}
		else {
			//ball1->Position.x = ball1->Position.x;
			ball1->Position.x += 0.1f;
		}
		
	}

	if (keys[GLFW_KEY_UP])
	{
		if (ball1->Position.y >= minTranslate) {
			ball1->Position.y -= 0.1f;
		}
		else {
			ball1->Position.y = ball1->Position.y;
		}
	}

	if (keys[GLFW_KEY_DOWN])
	{
		if (ball1->Position.y <= maxTranslate) {
			ball1->Position.y += 0.1f;
		}
		else {
			ball1->Position.y = ball1->Position.y;
		}
	}
	if (keys[GLFW_KEY_SPACE])
	{
		ball2->Velocity.x = 0.1f;
		ball2->Velocity.y = 0.1f;
		//ball1->Velocity.x = -0.1f;
	}

}

bool toggleTex() {
	if (curPositionX >= 0.0f) {
		return true;
	}
	else {
		return false;
	}
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0; i < 4; i++)
	{
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

Collision CheckCollisionAABB(BallObject &one, BallObject &two)
{
	//check difference between the balls'centers
	glm::vec2 difference = one.Position - two.Position;

	//if distance between centers <= 2*collision --> collision
	if (glm::length(difference) <= (2*one.Radius))
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

void DoCollisions()
{
	//check for collisions
	Collision collisionTest = CheckCollisionAABB(*ball1, *ball2);
	if (std::get<0>(collisionTest))
	{	
		//collision resolution
		Direction dir = std::get<1>(collisionTest);
		glm::vec2 diff_vec = std::get<2>(collisionTest);
		if (dir == LEFT || dir == RIGHT)
		{
			//ball 1 gets velocity
			ball1->Velocity.x = ball2->Velocity.x;
			ball1->Velocity.y = ball2->Velocity.y;
			//ball2
			ball2->Velocity.x = -ball2->Velocity.x;
			GLfloat penetration = ball2->Radius - std::abs(diff_vec.x);
			if (dir == LEFT)
				ball2->Position.x -= penetration;
			else
				ball2->Position.x += penetration;
		}
		else
		{
			//ball 1 gets velocity
			ball1->Velocity.x = ball2->Velocity.x;
			ball1->Velocity.y = ball2->Velocity.y;
			//ball2
			ball2->Velocity.y = -ball2->Velocity.y;
			GLfloat penetration = ball2->Radius - std::abs(diff_vec.y);
			if (dir == UP)
				ball2->Position.y -= penetration;
			else
				ball2->Position.y += penetration;
		}

	}
}


void Update(GLfloat dt,GLuint uniformModel)
{
	ball1->Move(dt, Width);

	ball2->Move(dt, Width);
	// Check for collisions
	DoCollisions();
	
}

int main()
{
	//Initialize window
	//mainWindow = Window(800, 800);
	mainWindow = Window(Width, Height);
	mainWindow.Initialise();

	CreatePlane();
	CreateShaders();

	//Load Textures
	greenTexture = Texture("Textures/green.png");
	greenTexture.LoadTextureA();
	redTexture = Texture("Textures/red.png");
	redTexture.LoadTextureA();
	whiteTexture = Texture("Textures/plain.png");
	whiteTexture.LoadTextureA();
	blueTexture = Texture("Textures/blue.png");
	blueTexture.LoadTextureA();

	//Import models from folder
	//disk = Model();
	//disk.LoadModel("Models/disk.obj");
	ball = Model();
	ball.LoadModel("Models/ball.obj");
	glm::vec2 ballPosition = glm::vec2(0.0f, 0.0f);
	
	ball1 = new BallObject();
	ball2 = new BallObject();
	ball1->Position.x = -2;
	ball1->Position.x = -2;
	//ball1->Velocity.x = 1;
	//ball1->Velocity.y = 1;


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
		//ball1->Position.x += 2.0f;

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


		//Floor plane
		glm::mat4 floorModelMat(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floorModelMat));
		greenTexture.UseTexture();
		meshList[0]->RenderMesh();
		
		//update & check for collisions
		Update(deltaTime, uniformModel);
		ball1->Draw(ball, uniformModel);
		ball2->Draw(ball, uniformModel);


		//Ball
		/*glm::mat4 ballModelMat(1.0f);
		ballModelMat = glm::translate(ballModelMat, glm::vec3(curPositionX, 1.0f, curPositionZ));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ballModelMat));
		ball.RenderModel();*/

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}