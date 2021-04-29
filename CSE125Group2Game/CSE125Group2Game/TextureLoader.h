#pragma once
#include <string>
#include <vector>

class TextureLoader;

// An opaque handle to a loaded texture
class Texture {
 public:
  friend TextureLoader;

  Texture() : mHandle(-1) {}

  bool isValid() const { return mHandle != -1; }

 private:
  Texture(int handle) : mHandle(handle) {}

  int mHandle;
};

class TextureLoader {
 private:
  // inner class definitions
  // represents a texture on the graphics card, including its gl id, etc.
  class GLTexture {
   public:
    GLTexture(unsigned char* data, int width, int height);
    // TODO: rule of five.
    ~GLTexture();

    void use() const;

   private:
    unsigned int mId;
  };

 public:
  Texture loadTexture(const std::string& filePath);
  void use(const Texture& texture) const;

 private:
  std::vector<GLTexture> mTextures;
};
