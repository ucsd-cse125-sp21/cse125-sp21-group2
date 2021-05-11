#pragma once

#include <conio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <irrKlang.h>

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class Sound {
public:
	const char* shootingSoundPath = "../Shared/Assets/Sounds/shooting.mp3";

	Sound();
	void play(char* filePath);
	void pausepAllSound();
	void stopAllSound();
private:
	irrklang::ISoundEngine* engine;

};