#pragma once

#include "../../Library/gameutil.h"

class StartLayout {
public:
	void Init();
	void Reset();
	void Show();

private:
	static const std::string RESOURCES_DIR;
	static const int TITLE_IMAGE_COUNT = 57;

	game_framework::CMovingBitmap background;
	game_framework::CMovingBitmap title;
};