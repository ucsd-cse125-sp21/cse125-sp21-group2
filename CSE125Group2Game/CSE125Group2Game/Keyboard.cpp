#include "Actions.hpp"

#include "Keyboard.h"

Keyboard::Keyboard(GLFWwindow *window)
{
    glfwSetKeyCallback(window, KeyMatching);
}

void Keyboard::poll()
{
    glfwPollEvents();
}