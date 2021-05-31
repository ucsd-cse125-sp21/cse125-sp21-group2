#include "Font.h"

#include <glad/glad.h>

#include "Utils.h"

Font::Font(const std::string& fontPath, TextureLoader& tloader) {
  // init freetype library
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    CRASH("Failed to init freetype library");
  }

  FT_Face face;
  if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
    CRASH("Failed to load font " + fontPath);
  }

  FT_Set_Pixel_Sizes(face, 0, 48);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // allow byte alignment

  for (unsigned char c = 0; c < 128; c++) {
    // load glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      CRASH("Failed to load font glyph :(");
    }

    // generate texture for the glyph
    Texture tex = tloader.loadGlyph(face);

    Character character = {
        tex, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x};

    mCharMap.emplace(c, character);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  // generate array for the quads
  // probably should refactor out into like a quad thing
  glGenVertexArrays(1, &mVao);
  glGenBuffers(1, &mVbo);

  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, 0, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Character Font::GetCharacter(char c) const { return mCharMap.at(c); }

float Font::GetStringWidth(const std::string& text, float scale) {
  float result = 0.0;

  for (auto c = text.begin(); c != text.end(); c++) {
    Character character = GetCharacter(*c);

    result += (character.nextOffset >> 6) *
              scale;  // bitshift by 6 to get value in pixels (2^6 = 64)
  }

  return result;
}
