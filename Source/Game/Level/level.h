#pragma once

#include <queue>
#include <stack>
#include "../../Library/gameutil.h"
#include "object.h"
#include "level_status_animation.h"
#include "../util/button.h"

class Level {
public:
	void Init();
	void Clear();

	void LoadLevel(int level);

	bool IsReachGoal();

	void SetBackButtonOnClick(Button::OnClickFunc func);
	void SetMusicButtonOnClick(Button::OnClickFunc func);
	void SetSoundButtonOnClick(Button::OnClickFunc func);
	void CheckMouseClick(POINT point);

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

	struct MoveableInfo {
		POINT position;
		bool up;
		bool down;
		bool left;
		bool right;
	};

	static std::string RESOURCES_DIR;
	static std::vector<game_framework::CMovingBitmap> levelBackgrounds;

	queue<Direction> moveBuffer;
	LevelStatusDisplay statusDisplay;

	game_framework::CMovingBitmap background;
	Button backButton;
	Button musicButton;
	Button soundButton;
	
	std::vector<std::vector<bool>> blockReachable;
	std::vector<MoveableInfo> checkedMovableBox;
	bool isDead = false;

	int gameboardWidth = 0, gameboardHeight = 0;
	std::vector<std::vector<int>> gameboard;
	std::vector<POINT> goals;
	Object bob;
	std::vector<Object> holes;
	std::vector<Object> boxs;

	std::stack<std::vector<UndoInfo>> undos;
	std::vector<UndoInfo> undoBuffer;
	std::vector<Object> filledObjectBuffer;

	void checkIsDead();
	void findReachableBlock();

	bool checkBoxMovable(Object &box, Direction direction);
	bool isBoxAlreadyMoveable(Object &box, Direction direction);
	int getBoxIndexInMoveableInfos(Object &box);
	
	bool isPointInsideGameboard(POINT position);
	bool isBoxSideReachable(Object &object, Direction direction);
	bool isBlockReachable(POINT position);
	bool isBlockEmpty(POINT position);
	bool isPointInsideObject(Object &object, POINT point);
	bool isBoxOnGoal(Object &box);
	bool isObjectOnIce(Object::Type type, POINT position);

	POINT getBoxRealPosition(POINT origionPosition, POINT gameboardPosition);

	void checkHoleFill();
	Object& findObjectAtPosition(Object::Type objectType, POINT position);

	std::vector<POINT> getObjectSidePositions(Object::Type type, POINT objectPosition, Direction direction);

	int moveObject(Object &object, Direction direction, int waitBlock);
	int getObjectMoveBlock(Object &object, Direction direction, int waitBlock);
	bool isObjectReachWall(Object::Type type, POINT position, Direction direction);
	bool isPointOnHoles(POINT position);
};