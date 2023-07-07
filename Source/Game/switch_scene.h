#pragma once

#include "../Library/gameutil.h"

class SwitchScene {
public:
	static void Init();
	
	static bool IsClosing();
	static bool IsOpening();

	static void CloseLeft();
	static void CloseRight();
	static void OpenLeft();
	static void OpenRight();

	static void Show();

private:
	static const std::string RESOURCES_DIR;
	static const int MAX_FRAME_COUNT = 13;
	static game_framework::CMovingBitmap closeLeft;
	static game_framework::CMovingBitmap closeRight;
	static game_framework::CMovingBitmap openLeft;
	static game_framework::CMovingBitmap openRight;
};