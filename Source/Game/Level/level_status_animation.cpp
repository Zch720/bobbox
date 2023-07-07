#include "stdafx.h"
#include "level_status_animation.h"
#include <string>

const std::string LevelStatusDisplay::RESOURCES_DIR = "./resources/animation/";

void LevelStatusDisplay::Init() {
	std::vector<std::string> clearImgs;
	std::vector<std::string> deadImgs;
	for (int i = 0; i < MAX_FRAME_COUNT; i++) {
		clearImgs.push_back(RESOURCES_DIR + "clear/" + to_string(i) + ".bmp");
		deadImgs.push_back(RESOURCES_DIR + "dead/" + to_string(i) + ".bmp");
	}
	clear.LoadBitmapByString(clearImgs, RGB(0x00, 0x00, 0xff));
	dead.LoadBitmapByString(deadImgs, RGB(0x00, 0x00, 0xff));
	clear.SetAnimation(30, true);
	dead.SetAnimation(30, true);
}

void LevelStatusDisplay::LevelStatusClean() {
	currentStatus = STATE_NONE;
}
	
void LevelStatusDisplay::StartClear() {
	currentStatus = CLEAR;
	clear.ToggleAnimation();
}

void LevelStatusDisplay::StartDead() {
	currentStatus = DEAD;
	dead.ToggleAnimation();
}

bool LevelStatusDisplay::IsShowing() {
	return currentStatus != STATE_NONE;
}

void LevelStatusDisplay::Show() {
	if (currentStatus == CLEAR) clear.ShowBitmap();
	if (currentStatus == DEAD) dead.ShowBitmap();
}