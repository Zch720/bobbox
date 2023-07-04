#include "stdafx.h"
#include "start.h"
#include <string>

const std::string StartLayout::RESOURCES_DIR = "./resources/layout/start/";

void StartLayout::Init() {
	background.LoadBitmapByString({ RESOURCES_DIR + "background.bmp"});

	std::vector<std::string> titleImgPath = {};
	for (int i = 0; i < TITLE_IMAGE_COUNT; i++) {
		titleImgPath.push_back(RESOURCES_DIR + "title/" + to_string(i) + ".bmp");
	}
	title.LoadBitmapByString(titleImgPath, RGB(0x00, 0xff, 0x00));
	title.SetTopLeft(630, 350);
	title.SetAnimation(45, false);
}

void StartLayout::Reset() {
	title.SetFrameIndexOfBitmap(0);
}

void StartLayout::Show() {
	background.ShowBitmap();
	title.ShowBitmap();
}