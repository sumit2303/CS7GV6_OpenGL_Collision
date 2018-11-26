#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "BallObject.h"
#include "Shader.h"
#include <stdlib.h> //abs

const float toRadians = 3.14159265f / 180.0f;

BallObject::BallObject()
	:Position(0.0f, 0.0f), Radius(1.0f), Size(glm::vec2(1, 1)), Velocity(0.0f),
	Rotation(0.0f)
{
}

BallObject::BallObject(glm::vec2 pos, glm::vec2 size, /*Texture2D sprite,*/ glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Velocity(velocity),
	Rotation(0.0f)
{
}

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
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

glm::vec2 BallObject::Gravity(GLfloat dt, GLuint window_width)
{

	this->Position += this->Velocity * dt;
	this->Velocity += glm::vec2(0, 2) * dt;
	this->Position += this->Velocity * dt;

	return this->Position;
}

//Apply transforms and render ball model
void BallObject::Draw(Model ball, GLuint uniformModel)
{
	glm::mat4 ballModelMat(1.0f);
	ballModelMat = glm::translate(ballModelMat, glm::vec3(this->Position.x, 1.0f, this->Position.y));
	if (rotationOn) {
		ballModelMat = glm::rotate(ballModelMat, toRadians * Rotation, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	ballModelMat = glm::rotate(ballModelMat, toRadians * 90.0f, glm::vec3(1.0f,0.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ballModelMat));
	ball.RenderModel();
}
