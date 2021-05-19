#include "TextureLoader.h"

#include <glad/glad.h>

#include "Utils.h"
#include "stb_image.h"

// Defines the names of files for the faces of the cubemap
std::vector<std::string> cubeMapFaceNames{
    "right.png", "left.png", "bottom.png", "top.png", "front.png", "back.png"};

// TODO: refactor this out of ctor, make GLTexture just a data class probably?
TextureLoader::GLTexture::GLTexture(unsigned char* data, int width,
                                    int height) {
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
  // create the opengl texture
  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  GLenum error;

  // set it to repeat on wrap, shouldn't really matter in the end
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // set up tri-linear filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // copy the image data into the texture buffer
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  // generate mipmaps for the texture
  // glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindTexture(GL_TEXTURE_2D, 1);
}

// TODO:
TextureLoader::GLTexture::~GLTexture() { /*glDeleteTextures(1, &mId);*/
}

void TextureLoader::GLTexture::use(unsigned int textureType) const {
  glBindTexture(textureType, mId);
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
  return Texture(mTextures.size() - 1, GL_TEXTURE_2D);
}

Texture TextureLoader::loadCubeMap(const std::string& directory) {
  // create the gl texture
  GLTexture texture;
  glGenTextures(1, &texture.mId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture.mId);

  // load each face of the cube map
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data;
  for (unsigned int i = 0; i < cubeMapFaceNames.size(); i++) {
    data = stbi_load((directory + "/" + cubeMapFaceNames[i]).c_str(), &width,
                     &height, &nrChannels, 0);

    // crash if we cant load the texture
    if (!data) {
      CRASH("Couln't load texture " + directory + "/" + cubeMapFaceNames[i] +
            "!");
    }

    // load the texture into the cubemap
    // TODO: This uses RGB format cause jpg, other use RGBA cause png. Probably
    // should just use png here too shrug
    // TODO: Could break things if use file format that is wrong
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  }

  // set texture filtering and wrapping
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  mTextures.push_back(texture);
  return Texture(mTextures.size() - 1, GL_TEXTURE_CUBE_MAP);
}

void TextureLoader::use(const Texture& texture) const {
  mTextures[texture.mHandle].use(texture.mTextureType);
}
