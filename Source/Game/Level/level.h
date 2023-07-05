#pragma once

#include "../../Library/gameutil.h"

class Level {
public:
	void Init();

	void LoadLevel(int level);

	void Show();

private:
	static std::string RESOURCES_DIR;
	static std::vector<game_framework::CMovingBitmap> levelBackgrounds;

	game_framework::CMovingBitmap background;
};