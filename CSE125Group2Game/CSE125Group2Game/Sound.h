#pragma once

#include <conio.h>
#include <irrKlang.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "GameObject.h"

#pragma comment(lib, "irrKlang.lib")  // link with irrKlang.dll

class Sound {
 public:
     const char* shootingSoundPath = "../Shared/Assets/Sounds/laser.mp3";
     const char* towerCollapseSoundPath = "../Shared/Assets/Sounds/collapse.mp3";
     const char* backgroundMusicPath = "../Shared/Assets/Sounds/background.mp3";

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
  void playBackgroundMusic(const char* filePath);
  void pauseBackgroundMusic();
  void deleteFromSoundVector(GameObject* obj);

  ~Sound() { 
      if (backgroundSound) {
          backgroundSound->stop();
          backgroundSound->drop();
      }
      engine->drop();
      delete engine;
  }

 private:
  irrklang::ISoundEngine* engine;

  irrklang::ISound* backgroundSound;

  std::vector<std::string> mSoundVector;

  std::vector<irrklang::ISound*> mSoundObjectVector;

};
