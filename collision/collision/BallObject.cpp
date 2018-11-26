#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "BallObject.h"
#include "Shader.h"
#include <stdlib.h> //abs

const float toRadians = 3.14159265f / 180.0f;

BallObject::BallObject()
	:Position(0.0f, 0.0f), Radius(1.0f), Size(glm::vec2(1, 1)), Velocity(0.0f), Color(1.0f),
	Rotation(0.0f)
{
}

BallObject::BallObject(glm::vec2 pos, glm::vec2 size, /*Texture2D sprite,*/ glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Velocity(velocity), Color(color),
	Rotation(0.0f)
{
}

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	// Move the balll
	/*if(this->Velocity.x > 0.01f)
		this->Velocity.x = 0.01f;
	if (this->Velocity.x < -0.01f)
		this->Velocity.x = -0.01f;
	if (this->Velocity.y > 0.01f)
		this->Velocity.y = 0.01f;
	if (this->Velocity.y < -0.01f)
		this->Velocity.y = -0.01f;
		*/
	
	this->Position += this->Velocity * dt;
	if (this->Position.x - this->Size.x <= -maxWidth)
	{
		this->Velocity.x = -this->Velocity.x;
		this->Position.x = -maxWidth + this->Size.x;
	}
	else if (this->Position.x + this->Size.x >= maxWidth)
	{
		this->Velocity.x = -this->Velocity.x;
		this->Position.x = maxWidth - this->Size.x;
	}
	if (this->Position.y - this->Size.y <= -maxWidth)
	{
		this->Velocity.y = -this->Velocity.y;
		this->Position.y = -maxWidth + this->Size.y;
	}
	else if (this->Position.y + this->Size.y >= maxWidth)
	{
		this->Velocity.y = -this->Velocity.y;
		this->Position.y = maxWidth - this->Size.y;
	}
	this->Position += this->Velocity;

	return this->Position;
}

//Apply transforms and render ball model
void BallObject::Draw(Model ball, GLuint uniformModel)
{
	glm::mat4 ballModelMat(1.0f);
	ballModelMat = glm::translate(ballModelMat, glm::vec3(this->Position.x, 1.0f, this->Position.y));
	ballModelMat = glm::rotate(ballModelMat, toRadians * Rotation, glm::vec3(Velocity.x, Velocity.y, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ballModelMat));
	ball.RenderModel();
}
