#include "Sound.h"


Sound::Sound() {
	engine = irrklang::createIrrKlangDevice();
	if (!engine)
		throw "Sound engine not supported!";
}

void Sound::play(char* filePath) {
	// Play Sound Once
	engine->play2D(filePath, false);
}