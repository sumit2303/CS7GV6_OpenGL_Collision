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
#include "BallObject.h"
#include "Model.h"

const float toRadians = 3.14159265f / 180.0f;
const GLfloat BALL_RADIUS = 1.0f;

Window mainWindow;
//window size
GLuint Width = 800;
GLuint Height = 800;

std::vector<Shader> shaderList;
std::vector<BallObject*> ballList;
std::vector <glm::mat4> modelMatrices;

Texture greenTexture;
Texture redTexture;
Texture whiteTexture;
Texture blueTexture;

Model ball;

DirectionalLight mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//ball object
BallObject *ball1, *ball2;

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
	if (keys[GLFW_KEY_RIGHT])
	{
		if (ball1->Position.x <= maxTranslate) {
			ball1->Position.x += 0.1f;
		}
		else {
			ball1->Position.x = ball1->Position.x;
		}
	}

	if (keys[GLFW_KEY_LEFT])
	{
		if (ball1->Position.x >= minTranslate) {
			ball1->Position.x -= 0.1f;
		}
		else {
			ball1->Position.x = ball1->Position.x;
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
		/*curPositionX += INITIAL_BALL_VELOCITY[0];
		curPositionZ += INITIAL_BALL_VELOCITY[1];*/
		ball2->Velocity.x = 0.1f;
	}

}

// Can delete maybe
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

GLboolean CheckCollisionAABB(BallObject &one, BallObject &two)
{
	// Get center point circle first
	glm::vec2 center1(one.Position + one.Radius);
	// Calculate AABB info (center, half-extents)
	//glm::vec2 aabbHalfExtents(two.Size.x / 2, two.Size.y / 2);
	//glm::vec2 aabbCenter(two.Position + aabbHalfExtents);
	glm::vec2 center2(two.Position + two.Radius);
	glm::vec2 difference = center1 - center2;

	glm::vec2 clamped = glm::clamp(difference, -center2, center2);
	// Add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec2 closest = center2 + clamped;
	difference = closest - center1;
	if (glm::length(difference) <= one.Radius)
	{
		//return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
		two.Size *= 2;
		//two.Position.x += 0.2f;
	}
	else
		return false;

	//return false;
}

void DoCollisions()
{
	//check for collisions
	collisionTest = CheckCollisionAABB(*ball1, *ball2);
}


void Update(GLfloat dt, GLuint uniformModel)
{
	ball2->Move(dt, Width);
	//ball1->Draw(ball, uniformModel);
	//ball2->Draw(ball, uniformModel);
	// Check for collisions
	//DoCollisions();
	//ball1->Position.x = 3.0f;
	//ball1->Draw(ball, uniformModel);
	//ball2->Draw(ball, uniformModel);

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
	redTexture = Texture("Textures/red.png");
	redTexture.LoadTextureA();
	whiteTexture = Texture("Textures/plain.png");
	whiteTexture.LoadTextureA();
	blueTexture = Texture("Textures/blue.png");
	blueTexture.LoadTextureA();


	//Import models from folder
	ball = Model();
	ball.LoadModel("Models/ball.obj");
	glm::vec2 ballPosition = glm::vec2(0.0f, 0.0f);
	//ball1 = new BallObject(ballPosition, glm::vec2(1,1), glm::vec3(1,1,1));
	ball1 = new BallObject();
	ball2 = new BallObject();


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


		//Floor plane
		glm::mat4 floorModelMat(1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floorModelMat));
		greenTexture.UseTexture();
		floorMesh.RenderMesh();

		//Ball - spawn
		if (mainWindow.mousePressed) {
			std::cout << "[WORLD COORDS]mouse at: " << modelPoint1.x << ", " << modelPoint1.y << ", " << modelPoint1.z << ", " << std::endl;
			BallObject* addBall = new BallObject();
			ballList.push_back(addBall);
			glm::mat4 newModelMat(1.0f);
			newModelMat = glm::translate(newModelMat, glm::vec3(modelPoint1.x, modelPoint1.y, modelPoint1.z));
			modelMatrices.push_back(newModelMat);
			// Need to turn off mouse pressed here because by the time it comes back to here on the next loop...
			// ...it will have logged 3 clicks
			mainWindow.mousePressed = !mainWindow.mousePressed;
		}
		//for ball in vector of ball objects
		//Render with nested for loop calling the model matrix for that object
		//}*/
		glm::mat4 newModelMat(1.0f);
		for (int i = 0; i < ballList.size(); i++)
			ballList[i]->Draw(ball, uniformModel, modelMatrices[i]);
	
		// update & check for collisions
		//ball2->Position += ball2->Velocity;
		Update(deltaTime, uniformModel);
		//ball1->Draw(ball, uniformModel);
		//ball2->Draw(ball, uniformModel);
		
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}