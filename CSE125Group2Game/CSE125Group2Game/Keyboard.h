#pragma once

#include <GLFW/glfw3.h>

class Keyboard
{
public:
  Keyboard(GLFWwindow *window);

  void poll();
};
