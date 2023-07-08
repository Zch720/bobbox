#pragma once

#include <string>

class AudioPlayer {
public:
	static void Init();

	static void PlayBGM();
	static void StopBGM();

	static bool GetMusicOnOff();
	static bool GetSoundOnOff();

	static void SetMusicOnOff(bool value);
	static void SwitchMusicOnOff();
	static void SetSoundOnOff(bool value);
	static void SwitchSoundOnOff();

	static void PlayWalkSound();
	static void PlayClearSound();
	static void PlayDeadSound();
	static void PlayTransitionSound();

private:
	enum AUDIO_ID {
		MUSIC_BGM,
		SOUND_WALK,
		SOUND_CLEAR,
		SOUND_DEAD,
		SOUND_TRANSITION,
	};

	static bool isMusicOn;
	static bool isSoundOn;
};