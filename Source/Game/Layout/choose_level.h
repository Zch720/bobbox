#pragma once

#include "../../Library/gameutil.h"
#include "../util/button.h"

class ChooseLevelLayout {
public:
	void Init();
	void Reset();
	void Show();

	void SetBackButtonOnClickFunc(Button::OnClickFunc func);

	void MouseMove(POINT point);
	void MouseClick(POINT point);

	void ChooserMoveLeft();
	void ChooserMoveRight();
	void ChooserMoveUp();
	void ChooserMoveDown();
	void ChooseLevel();

private:
	static const std::string RESOURCES_DIR;
	static const int LEVEL_WITHOUT_STORY_COUNT = 16;
	
	game_framework::CMovingBitmap background;
	game_framework::CMovingBitmap chooser;
	Button backButton;
	std::vector<Button> levelButtons = std::vector<Button>(18);
	std::vector<game_framework::CMovingBitmap> levelDones = std::vector<game_framework::CMovingBitmap>(18);
	std::vector<CPoint> chooserPositions = std::vector<CPoint>(18);
	int chooserIndex = 1;

	void setChooserPosition(int index);
};