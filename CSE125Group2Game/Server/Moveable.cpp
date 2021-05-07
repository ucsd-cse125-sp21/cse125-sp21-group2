﻿#include "Moveable.h"

#include <iostream>

void Moveable::moveForward() {
  mPivot->addRotation(glm::vec3(mRotationSpeed.x, 0, 0));
}

void Moveable::moveBack() {
  mPivot->addRotation(glm::vec3(-mRotationSpeed.x, 0, 0));
}

void Moveable::moveLeft() {
  mPivot->addRotation(glm::vec3(0, 0, mRotationSpeed.z));
}

void Moveable::moveRight() {
  mPivot->addRotation(glm::vec3(0, 0, -mRotationSpeed.z));
}

void Moveable::move(glm::vec3 angle) {
  if (glm::length(angle) == 0) {
    return;
  }
  std::cout << "pivot rotation : " << mPivot->getRotation().x << " "
            << mPivot->getRotation().y << " " << mPivot->getRotation().z
            << std::endl;

  std::cout << "angle: " << angle.x << " " << angle.y << " " << angle.z
            << std::endl;

  mPivot->addRotation(angle);

  // std::cout << "printing pivot's model: " << std::endl
  //          << mPivot->getModel()[0][0] << " " << mPivot->getModel()[0][1]
  //          << " " << mPivot->getModel()[0][2] << " "
  //          << mPivot->getModel()[0][3] << std::endl;
  // std::cout << mPivot->getModel()[1][0] << " " << mPivot->getModel()[1][1]
  //          << " " << mPivot->getModel()[1][2] << " "
  //          << mPivot->getModel()[1][3] << std::endl;
  // std::cout << mPivot->getModel()[2][0] << " " << mPivot->getModel()[2][1]
  //          << " " << mPivot->getModel()[2][2] << " "
  //          << mPivot->getModel()[2][3] << std::endl;
  // std::cout << mPivot->getModel()[3][0] << " " << mPivot->getModel()[3][1]
  //          << " " << mPivot->getModel()[3][2] << " "
  //          << mPivot->getModel()[3][3] << std::endl;

  glm::mat4 currModel = mTransform->getModel();

  // glm::mat4 newModel = currModel * mPivot->getModel();
  // glm::mat4 currModel = glm::mat4(
  //    glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0),
  //    glm::vec4(mTransform->getTranslation().x,
  //    mTransform->getTranslation().y,
  //              mTransform->getTranslation().z, 1));
  // glm::mat4 newModel = mPivot->getModel() * glm::inverse(currModel);
  // glm::mat4 newModel =
  //    glm::transpose(mPivot->getModel()) * glm::transpose(currModel);
  // glm::mat4 newModel = mPivot->getModel() * glm::transpose(currModel);
  glm::mat4 newModel = glm::transpose(mPivot->getModel()) * currModel;

  std::cout << "old: " << mTransform->getTranslation().x << " "
            << mTransform->getTranslation().y << " "
            << mTransform->getTranslation().z << std::endl;

  std::cout << "new model " << newModel[3][0] << " " << newModel[3][1] << " "
            << newModel[3][2] << std::endl;
  glm::vec3 newPos(newModel * glm::vec4(0, 0, 0, 1));
  // glm::vec3 newPos(glm::vec4(0, 0, 0, 1) * newModel);

  std::cout << "new: " << mTransform->getTranslation().x << " "
            << mTransform->getTranslation().y << " "
            << mTransform->getTranslation().z << std::endl;

  mTransform->setTranslation(newPos);
}

// glm::vec3 Moveable ::getWorldPositionFromPivot(glm::vec3 modelPos) {
//  return glm::vec3(0);
//  // TODO: figure out math
//
//  // glm::vec3(mPivot->getModel() * glm::mat4(1));
//}

glm::vec3 Moveable::getRotationSpeed() { return mRotationSpeed; }
