#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "BallObject.h"
#include "Shader.h"

BallObject::BallObject()
	:Position(0.0f, 0.0f), Radius(1.0f),Size(glm::vec2(1, 1)), Velocity(0.0f), Color(1.0f),
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
		this->Position = glm::vec2(0.0f, 0.0f);
		// Move the balll
		this->Position += this->Velocity * dt;
		if (this->Position.x <= 0)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = 0.0f;
		}
		else if (this->Position.x + this->Size.x >= window_width)
		{
			this->Velocity.x = -this->Velocity.x;
			this->Position.x = window_width - this->Size.x;
		}
		if (this->Position.y <= 0)
		{
			this->Velocity.y = -this->Velocity.y;
			this->Position.y = 0.0f;
		}
	
	return this->Position;
}

void BallObject::Draw(Model ball,GLuint uniformModel)
{
	glm::mat4 ballModelMat(1.0f);
	ballModelMat = glm::translate(ballModelMat, glm::vec3(this->Position.x, 1.0f, this->Position.y));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(ballModelMat));
	ball.RenderModel();
}
