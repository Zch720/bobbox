#include "stdafx.h"
#include "switch_scene.h"
#include <string>

const std::string SwitchScene::RESOURCES_DIR = "./resources/animation/";
game_framework::CMovingBitmap SwitchScene::closeLeft;
game_framework::CMovingBitmap SwitchScene::closeRight;
game_framework::CMovingBitmap SwitchScene::openLeft;
game_framework::CMovingBitmap SwitchScene::openRight;

void SwitchScene::Init() {
	std::vector<std::string> transfromLeftImgs;
	std::vector<std::string> transfromRightImgs;
	std::vector<std::string> clearImgs;
	for (int i = 0; i < MAX_FRAME_COUNT; i++) {
		transfromLeftImgs.push_back(RESOURCES_DIR + "transition_left/" + to_string(i) + ".bmp");
		transfromRightImgs.push_back(RESOURCES_DIR + "transition_right/" + to_string(i) + ".bmp");
		clearImgs.push_back(RESOURCES_DIR + "clear/" + to_string(i) + ".bmp");
	}
	openLeft.LoadBitmapByString(transfromLeftImgs, RGB(0x00, 0x00, 0xff));
	openRight.LoadBitmapByString(transfromRightImgs, RGB(0x00, 0x00, 0xff));

	std::reverse(transfromLeftImgs.begin(), transfromLeftImgs.end());
	std::reverse(transfromRightImgs.begin(), transfromRightImgs.end());
	closeLeft.LoadBitmapByString(transfromRightImgs, RGB(0x00, 0x00, 0xff));
	closeRight.LoadBitmapByString(transfromLeftImgs, RGB(0x00, 0x00, 0xff));

	closeLeft.SetAnimation(30, true);
	closeRight.SetAnimation(30, true);
	openLeft.SetAnimation(30, true);
	openRight.SetAnimation(30, true);
}

bool SwitchScene::IsClosing() {
	return closeRight.IsAnimation() || closeLeft.IsAnimation();
}

bool SwitchScene::IsOpening() {
	return openLeft.IsAnimation() || openRight.IsAnimation();
}

void SwitchScene::CloseLeft() {
	closeLeft.ToggleAnimation();
}

void SwitchScene::CloseRight() {
	closeRight.ToggleAnimation();
}

void SwitchScene::OpenLeft() {
	openLeft.ToggleAnimation();
}

void SwitchScene::OpenRight() {
	openRight.ToggleAnimation();
}

void SwitchScene::Show() {
	if (closeLeft.IsAnimation()) closeLeft.ShowBitmap();
	if (closeRight.IsAnimation()) closeRight.ShowBitmap();
	if (openLeft.IsAnimation()) openLeft.ShowBitmap();
	if (openRight.IsAnimation()) openRight.ShowBitmap();
}