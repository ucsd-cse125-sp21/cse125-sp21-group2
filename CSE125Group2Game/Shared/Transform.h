#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class Transform {
 public:
  Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

  Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale,
            glm::vec4 boundingBox);

  Transform(glm::vec3 translation, glm::quat rotation, glm::vec3 scale,
            glm::vec4 boundingBox);

  void addRotation(glm::vec3 degrees);

  void setRotation(glm::vec3 degrees);

  void setRotation(glm::quat quaternion);

  void addTranslation(glm::vec3 translation);

  void setTranslation(glm::vec3 translation);

  void addScale(glm::vec3 scale);

  void setScale(glm::vec3 scale);

  void setModel(glm::mat4 model);

  glm::vec3 getTranslation();

  glm::quat getRotation();

  glm::vec3 getScale();

  glm::mat4 getModel();

  glm::vec4 getBBox();

 private:
  glm::vec3 mTranslation;
  glm::quat mRotation;
  glm::vec3 mScale;

  float mBBLeft;
  float mBBRight;
  float mBBTop;
  float mBBBottom;

  glm::mat4 mModel;

  void updateModel();
};
