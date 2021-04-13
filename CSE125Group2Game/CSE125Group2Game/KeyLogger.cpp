#include "Actions.hpp"

#include "KeyLogger.h"

KeyLogger::KeyLogger(GLFWwindow *window) {
    glfwSetKeyCallback(window, KeyMatching);
}

void KeyLogger::poll() {
    glfwPollEvents();
}