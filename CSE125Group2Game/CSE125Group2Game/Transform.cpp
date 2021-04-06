#include "Transform.h"

#include <glm/gtx/quaternion.hpp>

Transform::Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) :
	mTranslation(translation), mRotation(rotation), mScale(scale), mModel(glm::mat4(1)) {}

void Transform::addRotation(glm::vec3 degrees) {
	mRotation = glm::quat(glm::radians(degrees)) * mRotation;
	updateModel();
}

void Transform::setRotation(glm::vec3 degrees) {
	mRotation = glm::quat(glm::radians(degrees));
	updateModel();
}

void Transform::addTranslation(glm::vec3 translation) {
	mTranslation += translation;
	updateModel();
}

void Transform::setTranslation(glm::vec3 translation) {
	mTranslation = translation;
	updateModel();
}

void Transform::addScale(glm::vec3 scale) {
	mScale += scale;
	updateModel();
}

void Transform::setScale(glm::vec3 scale) {
	mScale = scale;
	updateModel();
}

glm::vec3 Transform::getTranslation() {
	return mTranslation;
}

glm::quat Transform::getRotation() {
	return mRotation;
}

glm::vec3 Transform::getScale() {
	return mScale;
}

glm::mat4 Transform::getModel() {
	return mModel;
}

void Transform::updateModel() {
	mModel = glm::translate(glm::mat4(1.0f), mTranslation) * glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}