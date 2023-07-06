#pragma once

#include <queue>
#include <stack>
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
	void Undo();

	void Update();
	void Show();

private:
	enum UndoType {
		UNDO_UP,
		UNDO_DOWN,
		UNDO_LEFT,
		UNDO_RIGHT,
		UNDO_FILL
	};

	struct UndoInfo {
		UndoType type;
		Object::Type objectType;
		POINT objectFinalPosition;
		int moveBlock;
	};

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

	std::stack<std::vector<UndoInfo>> undos;
	std::vector<UndoInfo> undoBuffer;
	std::vector<Object> filledObjectBuffer;

	bool isPointInsideObject(Object &object, POINT point);
	bool isObjectOnIce(Object::Type type, POINT position);

	POINT getBoxRealPosition(POINT origionPosition, POINT gameboardPosition);

	void checkHoleFill();
	Object& findObjectAtPosition(Object::Type objectType, POINT position);

	int moveObject(Object &object, Direction direction, int waitBlock);
	int getMoveBobBlock(Object &object, Direction direction, int waitBlcok);
	int getMoveSmallBoxBlock(Object &object, Direction direction, int waitBlock);
	int getMoveMediumBoxBlock(Object &object, Direction direction, int waitBlock);
	int getMoveMediumBoxBlockHorizontal(Object &object, Direction direction, int waitBlock);
	int getMoveMediumBoxBlockVertical(Object &object, Direction direction, int waitBlock);
	bool isObjectReachWall(Object &object, Direction direction);
	bool isBobReachWall(POINT position, Direction direction);
	bool isSmallBoxReachWall(POINT position, Direction direction);
	bool isMediumBoxReachWall(POINT position, Direction direction);
};