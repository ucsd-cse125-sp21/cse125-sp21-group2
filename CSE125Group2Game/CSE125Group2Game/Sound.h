#pragma once

#include <conio.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <irrKlang.h>

#include "GameObject.h"

#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

class Sound {
public:
	const char* shootingSoundPath = "../Shared/Assets/Sounds/laser.mp3";

	Sound();
	void play(const char* filePath, irrklang::vec3df positionOfSoundOrigin);

	/*
		if toPause is true, it will pause all sounds
		else, it will unpause all sounds
	*/
	void pauseAllSounds(bool toPause);

	void stopAllSounds();
	void setListenerPosition(Transform* playerTransform);
	void playAccordingToGameObject(GameObject* obj);

private:
	irrklang::ISoundEngine* engine;
	std::vector<std::string> mSoundVector;
};