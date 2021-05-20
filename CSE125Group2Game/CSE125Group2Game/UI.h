#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>

#include "TextureLoader.h"

struct Character {
  Texture tex;
  glm::ivec2 size;
  glm::ivec2 bearing;
  unsigned int nextOffset;
};

// Just allow text ui for now, other stuff can come later...
class UI {
 public:
  UI(const std::string& fontPath, TextureLoader& tloader);

  glm::mat4 mProjection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

  unsigned int mVao, mVbo;

  Character GetCharacter(char c) const;

 private:
  std::unordered_map<char, Character> mCharMap;
};
