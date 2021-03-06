#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "AABB.h"

class Transform {
 public:
  Transform() : mTranslation(0.0f), mRotation(), mScale(1.0f) { updateModel(); }
  Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
  Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
            glm::vec3 boundingBox);
  Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale,
            glm::vec3 boundingBox);
  Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale);

  // identity transform factory method
  static Transform Identity() { return Transform(); }

  void addRotation(glm::vec3 degrees, bool localRot = false);
  void setRotation(glm::vec3 degrees);
  void setRotation(glm::quat quaternion);

  void addTranslation(glm::vec3 translation);
  void setTranslation(glm::vec3 translation);

  void addScale(glm::vec3 scale);
  void setScale(glm::vec3 scale);

  void setModel(glm::mat4 model);

  glm::vec3 getTranslation();
  glm::vec3 getModelTranslation();
  glm::quat getRotation();
  glm::vec3 getScale();
  glm::mat4 getModel() const;
  glm::vec3 getBBoxLens() const;
  AABB getBBox() const;

 private:
  glm::vec3 mTranslation;
  glm::quat mRotation;
  glm::vec3 mScale;

  // Length, height, width
  glm::vec3 mBoundingBoxLens;

  glm::mat4 mModel;

  void updateModel();
};
