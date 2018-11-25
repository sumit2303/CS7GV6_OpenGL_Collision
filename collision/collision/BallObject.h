#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Model.h"
#include "texture.h"
#include "Shader.h"

const float maxWidth = 10.0f;

class BallObject
{

public:
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;
	GLfloat Radius;
	GLfloat Rotation;


	BallObject();
	BallObject(glm::vec2 pos, glm::vec2 size,/* Texture2D sprite,*/
		glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f));
	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void Draw(Model Ball, GLuint uniformModel);
};
