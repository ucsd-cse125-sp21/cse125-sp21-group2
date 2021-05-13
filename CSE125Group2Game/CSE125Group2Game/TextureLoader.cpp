#include "TextureLoader.h"

#include <glad/glad.h>

#include "Utils.h"
#include "stb_image.h"

TextureLoader::GLTexture::GLTexture(unsigned char* data, int width,
                                    int height) {
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  // create the opengl texture
  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  GLenum error;
  error = glGetError();

  // set it to repeat on wrap, shouldn't really matter in the end
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  error = glGetError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  error = glGetError();

  // set up tri-linear filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  error = glGetError();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  error = glGetError();

  // copy the image data into the texture buffer
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  error = glGetError();

  // generate mipmaps for the texture
  // glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  error = glGetError();
  glBindTexture(GL_TEXTURE_2D, 1);
  error = glGetError();
}

// TODO:
TextureLoader::GLTexture::~GLTexture() { /*glDeleteTextures(1, &mId);*/
}

void TextureLoader::GLTexture::use() const {
  glBindTexture(GL_TEXTURE_2D, mId);
}

Texture TextureLoader::loadTexture(const std::string& filePath) {
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
      stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    CRASH("Couln't load texture " + filePath + "!");
  }

  mTextures.push_back(GLTexture(data, width, height));
  return Texture(mTextures.size() - 1);
}

void TextureLoader::use(const Texture& texture) const {
  mTextures[texture.mHandle].use();
}
