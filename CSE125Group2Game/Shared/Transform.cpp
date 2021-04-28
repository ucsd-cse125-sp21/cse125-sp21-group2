#include "Transform.h"

#include <glm/gtx/quaternion.hpp>
#include <iostream>

Transform::Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
    : mTranslation(translation),
      mRotation(glm::quat(glm::radians(rotation))),
      mScale(scale),
      mModel(glm::mat4(1)) {
  updateModel();
}

Transform::Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
                     glm::vec3 boundingBox)
    : mTranslation(translation),
      mRotation(glm::quat(glm::radians(rotation))),
      mScale(scale),
      mModel(glm::mat4(1)),
      mBoundingBox(boundingBox) {
  updateModel();
}

Transform::Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
    : mTranslation(translation),
      mRotation(rotation),
      mScale(scale),
      mModel(glm::mat4(1)),
      mBoundingBox(NULL) {
  updateModel();
}

Transform::Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale,
                     glm::vec3 boundingBox)
    : mTranslation(translation),
      mRotation(rotation),
      mScale(scale),
      mModel(glm::mat4(1)),
      mBoundingBox(boundingBox) {
  updateModel();
}

void Transform::addRotation(glm::vec3 degrees) {
  mRotation = glm::quat(glm::radians(degrees)) * mRotation;
  updateModel();
}

void Transform::setRotation(glm::vec3 degrees) {
  mRotation = glm::quat(glm::radians(degrees));
  updateModel();
}

void Transform::setRotation(glm::quat quaternion) {
  mRotation = quaternion;
  updateModel();
}

void Transform::addTranslation(glm::vec3 translation) {
  mTranslation += translation;
  updateModel();
}

void Transform::setTranslation(glm::vec3 translation) {
  if (translation.x < -20) {
    std::cout << "oof < -20" << std::endl;
  }

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

void Transform::setModel(glm::mat4 model) { mModel = model; }

glm::vec3 Transform::getTranslation() {
  if (mTranslation.x < -20) {
    std::cout << "oof < -20 in get" << std::endl;
  }
  return mTranslation;
}

glm::quat Transform::getRotation() { return mRotation; }

glm::vec3 Transform::getScale() { return mScale; }

glm::mat4 Transform::getModel() { return mModel; }

glm::vec3 Transform::getBBox() { return mBoundingBox; }

void Transform::updateModel() {
  mModel = glm::translate(glm::mat4(1.0f), mTranslation) *
           glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}
