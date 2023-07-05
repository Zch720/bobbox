#include "stdafx.h"
#include "level.h"
#include <string>

std::string Level::RESOURCES_DIR = "./resources/level/";
std::vector<game_framework::CMovingBitmap> Level::levelBackgrounds = std::vector<game_framework::CMovingBitmap>(16);

void Level::Init() {
	for (int i = 0; i < 16; i++) {
		levelBackgrounds[i].LoadBitmapByString({ RESOURCES_DIR + "background_" + to_string(i) + ".bmp" });
	}
}

void Level::LoadLevel(int level) {
	background = levelBackgrounds[level - 1];
}

void Level::Show() {
	background.ShowBitmap();
}