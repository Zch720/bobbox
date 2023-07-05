#include "stdafx.h"
#include "choose_level.h"
#include <string>

const std::string ChooseLevelLayout::RESOURCES_DIR = "./resources/layout/chooselevel/";

void ChooseLevelLayout::Init() {
	background.LoadBitmapByString({ RESOURCES_DIR + "background.bmp"});
	chooser.LoadBitmapByString({ RESOURCES_DIR + "chooser.bmp"}, RGB(0x00, 0xff, 0x00));
	backButton.LoadBitmapByString({ RESOURCES_DIR + "back.bmp"});
	backButton.SetTopLeft(20, 20);

	levelButtons[0].LoadBitmapByString({ RESOURCES_DIR + "story.bmp"});
	levelButtons[17].LoadBitmapByString({ RESOURCES_DIR + "story.bmp"});
	for (int i = 1; i <= LEVEL_WITHOUT_STORY_COUNT; i++) {
		levelButtons[i].LoadBitmapByString({ RESOURCES_DIR + "level_" + to_string(i) + ".bmp"});
	}
	levelDones[0].LoadBitmapByString({ RESOURCES_DIR + "done.bmp"});
	for (int i = 1; i < 18; i++) {
		levelDones[i] = levelDones[0];
	}

	for (int i = 0, y = 0; i < 3; i++, y += 110) {
		for (int j = 0, x = 0; j < 6; j++, x += 110) {
			levelButtons[i * 6 + j].SetTopLeft(461 + x, 260 + y);
			levelDones[i * 6 + j].SetTopLeft(451 + x, 277 + y);
			chooserPositions[i * 6 + j] = {455 + x, 254 + y};
		}
	}
	setChooserPosition(1);
}

void ChooseLevelLayout::Reset() {
	setChooserPosition(1);
}

void ChooseLevelLayout::SetLastestDoneLevel(int level) {
	lastestDoneLevel = level;
}

void ChooseLevelLayout::SetBackButtonOnClickFunc(Button::OnClickFunc func) {
	backButton.SetOnClickFunc(func);
}

void ChooseLevelLayout::MouseMove(POINT point) {
	for (int i = 0; i < 18; i++) {
		if (levelButtons[i].IsHover(point)) {
			setChooserPosition(i);
			break;
		}
	}
}

int ChooseLevelLayout::GetMouseClickLevelButton(POINT point) {
	backButton.CheckMouseClick(point);

	if (levelButtons[chooserIndex].IsHover(point)) return chooserIndex;
	return -1;
}

void ChooseLevelLayout::ChooserMoveLeft() {
	if (chooserIndex == 0) return;
	setChooserPosition(chooserIndex - 1);
}

void ChooseLevelLayout::ChooserMoveRight() {
	if (chooserIndex == 17) return;
	setChooserPosition(chooserIndex + 1);
}

void ChooseLevelLayout::ChooserMoveUp() {
	setChooserPosition((chooserIndex + 12) % 18);
}

void ChooseLevelLayout::ChooserMoveDown() {
	setChooserPosition((chooserIndex + 6) % 18);
}

int ChooseLevelLayout::ChooseLevel() {
	return chooserIndex;
}

void ChooseLevelLayout::Show() {
	background.ShowBitmap();
	for (auto levelButton : levelButtons) {
		levelButton.ShowBitmap();
	}
	chooser.ShowBitmap();
	backButton.ShowBitmap();
	for (int i = 1; i <= lastestDoneLevel; i++) {
		levelDones[i].ShowBitmap();
	}
}

void ChooseLevelLayout::setChooserPosition(int index) {
	chooserIndex = index;
	CPoint position = chooserPositions[index];
	chooser.SetTopLeft(position.x, position.y);
}