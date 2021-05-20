#pragma once
#include "GameObject.h"

class Moveable : public GameObject {
 public:
  Moveable(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health) {
    initPivotModel(transform);
  }

  Moveable(Transform* transform, std::string name, int health, ObjectType type)
      : GameObject(transform, name, health, type) {
    initPivotModel(transform);
  }

  void move(glm::vec3 angle);
  void resetModel();

  Transform* mPivot;
  Transform* mModelTransform;

 private:
  void initPivotModel(Transform* transform);

  glm::mat4 mInitPivotModel;
};
