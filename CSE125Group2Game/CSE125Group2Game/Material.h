#pragma once
#include <assimp/material.h>

#include <glm/vec3.hpp>

#include "TextureLoader.h"

struct Material {
  glm::vec3 mAmbient;
  glm::vec3 mDiffuse;
  glm::vec3 mSpecular;
  float mShininess;

  // TODO: make optional?
  Texture diffuseMap;

  Material()
      : mAmbient(glm::vec3(1.0f, 0.0f, 0.0f)),
        mDiffuse(glm::vec3(1.0f, 0.0f, 0.0f)),
        mSpecular(glm::vec3(1.0f, 0.0f, 0.0f)),
        mShininess(32.0f) {}
  Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
           float shininess)
      : mAmbient(ambient),
        mDiffuse(diffuse),
        mSpecular(specular),
        mShininess(shininess) {}
  Material(aiMaterial* other, TextureLoader& tloader);
};
