#pragma once

#include <queue>
#include "../../Library/gameutil.h"
#include "object.h"

class Level {
public:
	void Init();
	void Clear();

	void LoadLevel(int level);

	bool IsReachGoal();
	bool IsInsideGameboard(POINT position);

	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

	void Update();
	void Show();

private:
	static std::string RESOURCES_DIR;
	static std::vector<game_framework::CMovingBitmap> levelBackgrounds;

	queue<Direction> moveBuffer;

	game_framework::CMovingBitmap background;

	int gameboardWidth = 0, gameboardHeight = 0;
	std::vector<std::vector<int>> gameboard;
	std::vector<POINT> goals;
	Object bob;
	std::vector<Object> holes;
	std::vector<Object> boxs;

	bool isPointInsizObject(Object &object, POINT point);

	POINT getBoxRealPosition(POINT origionPosition, POINT gameboardPosition);

	int moveObject(Object &object, Direction direction, int waitBlock);
	int getMoveBobBlock(Object &object, Direction direction, int waitBlcok);
	int getMoveSmallBoxBlock(Object &object, Direction direction, int waitBlock);
	bool isObjectReachWall(Object &object, Direction direction);
	bool isBobReachWall(POINT position, Direction direction);
	bool isSmallBoxReachWall(POINT position, Direction direction);
};