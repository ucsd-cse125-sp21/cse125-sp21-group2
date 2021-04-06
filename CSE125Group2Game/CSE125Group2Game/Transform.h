#pragma once

#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

	void addRotation(glm::vec3 degrees);

	void setRotation(glm::vec3 degrees);

	void addTranslation(glm::vec3 translation);

	void setTranslation(glm::vec3 translation);

	void addScale(glm::vec3 scale);

	void setScale(glm::vec3 scale);

	glm::vec3 getTranslation();

	glm::quat getRotation();

	glm::vec3 getScale();

	glm::mat4 getModel();

private:
	glm::vec3 mTranslation;
	glm::quat mRotation;
	glm::vec3 mScale;

	glm::mat4 mModel;

	void updateModel();
};

