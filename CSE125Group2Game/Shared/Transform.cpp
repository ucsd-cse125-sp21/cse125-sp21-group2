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
      mBoundingBoxLens(boundingBox) {
  updateModel();
}

Transform::Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
    : mTranslation(translation),
      mRotation(rotation),
      mScale(scale),
      mModel(glm::mat4(1)),
      mBoundingBoxLens(NULL) {
  updateModel();
}

Transform::Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale,
                     glm::vec3 boundingBox)
    : mTranslation(translation),
      mRotation(rotation),
      mScale(scale),
      mModel(glm::mat4(1)),
      mBoundingBoxLens(boundingBox) {
  updateModel();
}

void Transform::addRotation(glm::vec3 degrees, bool localRot) {
  if (!localRot)
    mRotation = glm::quat(glm::radians(degrees)) * mRotation;
  else
    mRotation = mRotation * glm::quat(glm::radians(degrees));
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

glm::vec3 Transform::getTranslation() { return mTranslation; }

glm::vec3 Transform::getModelTranslation() {
  return glm::vec3(mModel[3][0], mModel[3][1], mModel[3][2]);
}

glm::quat Transform::getRotation() { return mRotation; }

glm::vec3 Transform::getScale() { return mScale; }

glm::mat4 Transform::getModel() const { return mModel; }

glm::vec3 Transform::getBBoxLens() const { return mBoundingBoxLens; }

AABB Transform::getBBox() const {
  return AABB::FromPosLen(mTranslation, mBoundingBoxLens);
}

void Transform::updateModel() {
  mModel = glm::translate(glm::mat4(1.0f), mTranslation) *
           glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}
