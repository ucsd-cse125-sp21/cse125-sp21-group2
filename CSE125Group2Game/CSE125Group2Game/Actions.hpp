#include <iostream>
#include <GLFW/glfw3.h>

inline void KeyMatching(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch(key) {
        case GLFW_KEY_W:
            if (action == GLFW_REPEAT) {
                // TODO: Callback for key
            }
            break;
        case GLFW_KEY_A:
            if (action == GLFW_REPEAT) {
                // TODO: Callback for key
            }
            break;
        case GLFW_KEY_S:
            if (action == GLFW_REPEAT) {
                // TODO: Callback for key
            }
            break;
        case GLFW_KEY_D:
            if (action == GLFW_REPEAT) {
                // TODO: Callback for key
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS) {
                // TODO: Callback for key
                // For when key is press once
                std::cout << "space is pressed" << std::endl;
            } else if (action == GLFW_REPEAT) {
                // For when key is being held down
            }
            break;
    }
}