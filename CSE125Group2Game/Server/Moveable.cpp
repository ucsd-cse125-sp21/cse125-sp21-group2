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

#include <glm/gtx/euler_angles.hpp>

void Moveable::move(glm::vec3 angle) {
  /*std::cout << "pivot rotation : " << mPivot->getRotation().x << " "
            << mPivot->getRotation().y << " " << mPivot->getRotation().z
            << std::endl;

  std::cout << "angle: " << angle.x << " " << angle.y << " " << angle.z
            << std::endl;*/
  mPivot->setModel(mPivot->getModel() * glm::eulerAngleXYZ(angle.x / 100,
                                                           angle.y / 100,
                                                           angle.z / 100));

  // Childs model
  glm::mat4 currModel = mModelTransform->getModel();

  glm::mat4 newModel = mPivot->getModel() * currModel;

  /*std::cout << "old: " << mTransform->getTranslation().x << " "
            << mTransform->getTranslation().y << " "
            << mTransform->getTranslation().z << std::endl;

  std::cout << "new model " << newModel[3][0] << " " << newModel[3][1] << " "
            << newModel[3][2] << std::endl;*/
  glm::vec3 newPos(newModel * glm::vec4(0, 0, 0, 1));

  // Need a model cord transform, and a world cord transform which is in mWorld

  glm::mat4 finalModel = mPivot->getModel();
  finalModel[3][0] = newPos.x;
  finalModel[3][1] = newPos.y;
  finalModel[3][2] = newPos.z;

  mTransform->setModel(finalModel);
}

// glm::vec3 Moveable ::getWorldPositionFromPivot(glm::vec3 modelPos) {
//  return glm::vec3(0);
//  // TODO: figure out math
//
//  // glm::vec3(mPivot->getModel() * glm::mat4(1));
//}

glm::vec3 Moveable::getRotationSpeed() { return mRotationSpeed; }
