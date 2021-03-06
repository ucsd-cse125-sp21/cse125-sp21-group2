#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>
#include <vector>

class TextureLoader;

// An opaque handle to a loaded texture
class Texture {
 public:
  friend TextureLoader;

  Texture() : mHandle(-1), mTextureType(0) {}

  bool isValid() const { return mHandle != -1; }

 private:
  Texture(int handle, unsigned int type)
      : mHandle(handle), mTextureType(type) {}

  int mHandle;
  unsigned int mTextureType;
};

class TextureLoader {
 private:
  // inner class definitions
  // represents a texture on the graphics card, including its gl id, etc.
  class GLTexture {
   public:
    GLTexture(unsigned char* data, int width, int height);
    GLTexture() : mId(-1) {}
    // TODO: rule of five.
    ~GLTexture();

    void use(unsigned int textureType) const;
    unsigned int mId;
  };

 public:
  Texture loadTexture(const std::string& filePath);
  Texture loadCubeMap(const std::string& directory);
  Texture loadGlyph(const FT_Face& face);
  void use(const Texture& texture) const;

 private:
  std::vector<GLTexture> mTextures;

  std::unordered_map<std::string, Texture> mTextureMap;
};
