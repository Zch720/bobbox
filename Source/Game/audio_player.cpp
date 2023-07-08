#include "stdafx.h"
#include "../Library/audio.h"
#include "audio_player.h"

bool AudioPlayer::isMusicOn = true;
bool AudioPlayer::isSoundOn = true;

void AudioPlayer::Init() {
	game_framework::CAudio::Instance() -> Load(MUSIC_BGM, "./resources/audios/bgm.wav");
	game_framework::CAudio::Instance() -> Load(SOUND_WALK, "./resources/audios/walking.wav");
	game_framework::CAudio::Instance() -> Load(SOUND_CLEAR, "./resources/audios/clear.wav");
	game_framework::CAudio::Instance() -> Load(SOUND_DEAD, "./resources/audios/dead.wav");
	game_framework::CAudio::Instance() -> Load(SOUND_TRANSITION, "./resources/audios/transition.wav");
}

void AudioPlayer::PlayBGM() {
	game_framework::CAudio::Instance() -> Play(MUSIC_BGM, true);
}

void AudioPlayer::StopBGM() {
	game_framework::CAudio::Instance() -> Stop(MUSIC_BGM);
}

bool AudioPlayer::GetMusicOnOff() {
	return isMusicOn;
}

bool AudioPlayer::GetSoundOnOff() {
	return isSoundOn;
}

void AudioPlayer::SetMusicOnOff(bool value) {
	isMusicOn = value;
}

void AudioPlayer::SwitchMusicOnOff() {
	isMusicOn = !isMusicOn;
}

void AudioPlayer::SetSoundOnOff(bool value) {
	isSoundOn = value;
}

void AudioPlayer::SwitchSoundOnOff() {
	isSoundOn = !isSoundOn;
}

void AudioPlayer::PlayWalkSound() {
	if (isSoundOn) game_framework::CAudio::Instance() -> Play(SOUND_WALK);
}

void AudioPlayer::PlayClearSound() {
	if (isSoundOn) game_framework::CAudio::Instance() -> Play(SOUND_CLEAR);
}

void AudioPlayer::PlayDeadSound() {
	if (isSoundOn) game_framework::CAudio::Instance() -> Play(SOUND_DEAD);
}

void AudioPlayer::PlayTransitionSound() {
	if (isSoundOn) game_framework::CAudio::Instance() -> Play(SOUND_TRANSITION);
}