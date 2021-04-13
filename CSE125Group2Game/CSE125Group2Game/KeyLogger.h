#pragma once

#include <GLFW/glfw3.h>

class KeyLogger {
 public:
    KeyLogger(GLFWwindow *window);

    void poll();
};
