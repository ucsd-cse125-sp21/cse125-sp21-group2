#pragma once
#include <assimp/material.h>

#include <glm/vec3.hpp>

struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;

  Material()
      : ambient(glm::vec3(1.0f, 0.0f, 0.0f)),
        diffuse(glm::vec3(1.0f, 0.0f, 0.0f)),
        specular(glm::vec3(1.0f, 0.0f, 0.0f)),
        shininess(32.0f) {}
  Material(aiMaterial* other);
};
