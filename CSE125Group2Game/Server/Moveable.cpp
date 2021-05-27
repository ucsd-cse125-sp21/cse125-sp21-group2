#include "Moveable.h"

#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#include "GameLogicServer.h"

void Moveable::move(glm::vec3 angle) {
  angle = (GameLogicServer::getLogicServer()->mDeltaTime) * angle;
  mPivot->setModel(mPivot->getModel() *
                   glm::eulerAngleXYZ(angle.x, angle.y, angle.z));

  // Childs model
  glm::mat4 currModel = mModelTransform->getModel();

  glm::mat4 newModel = mPivot->getModel() * currModel;

  glm::vec3 newPos(newModel * glm::vec4(0, 0, 0, 1));

  // Hack to fix component translation
  mTransform->setTranslation(newPos);

  // DO NOT REMOVE
  mTransform->setModel(newModel);
}

void Moveable::initPivotModel(Transform* transform) {
  mPivot = new Transform(glm::vec3(0), transform->getRotation(), glm::vec3(1));

  mModelTransform = new Transform(transform->getTranslation(), glm::vec3(0),
                                  transform->getScale());

  move(glm::vec3(0));
  mInitPivotModel = mPivot->getModel();
}

void Moveable::resetModel() {
  mPivot->setModel(mInitPivotModel);
  move(glm::vec3(0));
}
