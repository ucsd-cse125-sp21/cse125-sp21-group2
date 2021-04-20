#pragma once
// main.cpp : This file contains the 'main' function. Program execution begins
// and ends there.
//

#include <iostream>

#include "GameManager.h"

/*
 * 1. Set up ticks, lets do 30 per second (frame rate still unbounded)
 * 2. Refactor some of the glfw window code into a window class
 * 3. Refactor singletons into a Game class or something (Alex)
 * 4. Refactor renderer class to be less bad (Evan)
 */
int main() {
  GameManager* gameManager = GameManager::getManager();

  gameManager->Update();
}
