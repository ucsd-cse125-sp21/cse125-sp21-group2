#include "ShaderProgram.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>

#include "Utils.h"

/**
 * Constructs a new shader program, using the vertex and fragment shaders
 * referred to by the passed in paths.
 *
 * @param vertexShaderPath The path to the vertex shader file.
 * @param fragmentShaderPath The path to the fragment shader file.
 * @throw std::exception Throws an exception when the passed in shaders are
 * unable to be compiled or linked (i.e. they are invalid).
 */
ShaderProgram::ShaderProgram(const std::string& vertexShaderPath,
                             const std::string& fragmentShaderPath) {
  // read the vertex shader from file
  std::ifstream vertexFile(vertexShaderPath);
  std::string vertShader((std::istreambuf_iterator<char>(vertexFile)),
                         std::istreambuf_iterator<char>());
  const char* vertCStr = vertShader.c_str();

  unsigned int vertId;
  vertId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertId, 1, &vertCStr, NULL);
  glCompileShader(vertId);

  // check if compilation was successful, and print error if not
  int success;
  char infoLog[512];
  glGetShaderiv(vertId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertId, 512, NULL, infoLog);
    std::cerr << "Vertex Shader \"" << vertexShaderPath
              << "\" Compilation Failed\n"
              << infoLog << std::endl;
    CRASH("Vertex shader " + vertexShaderPath + " compilation failed!");
  }

  // read the fragment shader from file
  std::ifstream fragmentFile(fragmentShaderPath);
  std::string fragShader((std::istreambuf_iterator<char>(fragmentFile)),
                         std::istreambuf_iterator<char>());
  const char* fragCStr = fragShader.c_str();

  unsigned int fragId;
  fragId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragId, 1, &fragCStr, NULL);
  glCompileShader(fragId);

  // check if compilation was successful, and print error if not
  glGetShaderiv(fragId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragId, 512, NULL, infoLog);
    std::cerr << "Fragment Shader \"" << fragmentShaderPath
              << "\" Compilation Failed\n"
              << infoLog << std::endl;
    CRASH("Fragment shader " + fragmentShaderPath + " compilation failed!");
  }

  // link the shader program
  mId = glCreateProgram();
  glAttachShader(mId, vertId);
  glAttachShader(mId, fragId);
  glLinkProgram(mId);

  glGetProgramiv(mId, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(mId, 512, NULL, infoLog);
    std::cerr << "Shader Link Failed\n" << infoLog << std::endl;
    CRASH("Shader program link failed!");
  }

  // delete shader objects, unnecessary now
  glDeleteShader(vertId);
  glDeleteShader(fragId);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : mId(other.mId) {}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
  if (this == &other) {
    return *this;
  }

  mId = other.mId;
  other.mId = 0;
}

ShaderProgram::~ShaderProgram() { glDeleteProgram(mId); }

/**
 * Use the shader program for rendering.
 *
 * Note that this implicitly replaces the
 * previous shader program used (i.e. only one can be used for any particular
 * render call).
 */
void ShaderProgram::use() { glUseProgram(mId); }
