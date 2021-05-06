#pragma once

#include <string>

class ShaderProgram {
 public:
  ShaderProgram(const std::string& vertexShaderPath,
                const std::string& fragmentShaderPath);
  ShaderProgram(const ShaderProgram& other) = delete;
  ShaderProgram(ShaderProgram&& other) noexcept;
  ShaderProgram& operator=(const ShaderProgram& other) = delete;
  ShaderProgram& operator=(ShaderProgram&& other) noexcept;
  ~ShaderProgram();

  void use();

  unsigned int getID() const { return mId; };

 private:
  unsigned int mId;
};
