#include "Sound.h"


Sound::Sound() {
	engine = irrklang::createIrrKlangDevice();
	if (!engine)
		throw "Sound engine not supported!";
}

void Sound::play(const char* filePath, irrklang::vec3df positionOfSoundOrigin) {
	// Play Sound Once
	engine->play3D(filePath, positionOfSoundOrigin, false);
}

void Sound::pauseAllSounds(bool toPause) {
	// Play Sound Once
	engine->setAllSoundsPaused(toPause);
}

void Sound::stopAllSounds() {
	engine->stopAllSounds();
}

void Sound::setListenerPosition(Transform* playerTransform) {
	if (playerTransform) {
	auto playerTranslation = playerTransform->getTranslation();
	irrklang::vec3df playerPosition(playerTranslation.x, playerTranslation.y, playerTranslation.z);
	glm::mat4 inverted = glm::inverse(playerTransform->getModel());
	glm::vec3 forward = normalize(glm::vec3(inverted[2]));
	irrklang::vec3df lookDirection(forward.x, forward.y, forward.z);
	irrklang::vec3df velocityPerSecond(1,1,1);
	irrklang::vec3df upVector(playerTranslation.x, playerTranslation.y, playerTranslation.z);

	engine->setListenerPosition(playerPosition, lookDirection, velocityPerSecond, upVector);
	}
}

void Sound::playAccordingToGameObject(GameObject* obj) {
	if (!obj) return;
	for (std::string objectName : mSoundVector) {
		if (obj->getName() == objectName) {
			return;
		}
	}

	mSoundVector.push_back(obj->getName());
	if (obj->isPlayer()) {

	}
	else if (obj->isEnemy()) {

	}
	else if (obj->isProjectile()) {
		irrklang::vec3d position(
			obj->getTransform()->getTranslation().x, 
			obj->getTransform()->getTranslation().y, 
			obj->getTransform()->getTranslation().z);

		play(Sound::shootingSoundPath, position);
	}
	else if (obj->isTower()) {

	}
}