#pragma once
#include "GameObject.h"

class Moveable : public GameObject {
 public:
  Moveable(Transform* transform, std::string name, int health)
      : GameObject(transform, name, health) {
    mPivot = new Transform(glm::vec3(0), transform->getRotation(),
                           transform->getScale());

    mModelTransform =
        new Transform(transform->getTranslation() / transform->getScale(),
                      glm::vec3(0), transform->getScale());
  }

  Moveable(Transform* transform, std::string name, int health, ObjectType type)
      : GameObject(transform, name, health, type) {
    mPivot = new Transform(glm::vec3(0), transform->getRotation(),
                           transform->getScale());

    mModelTransform =
        new Transform(transform->getTranslation() / transform->getScale(),
                      glm::vec3(0), transform->getScale());
  }

  void moveForward();

  void moveBack();

  void moveLeft();

  void moveRight();

  void move(glm::vec3 angle);

  glm::vec3 getWorldPositionFromPivot(glm::vec3 modelPos);

  glm::vec3 getRotationSpeed();

  glm::mat3 rotate(const float degrees, const glm::vec3& axis);

  Transform* mPivot;
  Transform* mModelTransform;

 protected:
  glm::vec3 mRotationSpeed = glm::vec3(-4, -8, 0);
};
