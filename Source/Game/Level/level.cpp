#include "stdafx.h"
#include "level.h"
#include <string>
#include <fstream>

std::string Level::RESOURCES_DIR = "./resources/level/";
std::vector<game_framework::CMovingBitmap> Level::levelBackgrounds = std::vector<game_framework::CMovingBitmap>(16);

void Level::Init() {
	Object::Init();
	statusDisplay.Init();
	for (int i = 0; i < 16; i++) {
		levelBackgrounds[i].LoadBitmapByString({ RESOURCES_DIR + "background_" + to_string(i) + ".bmp" });
	}
}

void Level::Clear() {
	while (!moveBuffer.empty()) moveBuffer.pop();
	gameboard.clear();
	goals.clear();
	holes.clear();
	boxs.clear();
	statusDisplay.LevelStatusClean();
}

void Level::LoadLevel(int level) {
	background = levelBackgrounds[level - 1];

	std::ifstream dataFile(RESOURCES_DIR + to_string(level - 1) + ".level");
	POINT textureOriginPosition{0, 0};
	std::string buffer;

	dataFile >> buffer>> gameboardWidth>> gameboardHeight;
	dataFile >> buffer >> textureOriginPosition.x >> textureOriginPosition.y;

	for (int w = 0; w < gameboardWidth; w++) 
		gameboard.push_back(std::vector<int>(gameboardHeight));

	dataFile >> buffer;
	for (int h = 0; h < gameboardHeight; h++) {
		for (int w = 0; w < gameboardWidth; w++) {
			dataFile >> gameboard[w][h];
		}
	}

	int goalCount;
	dataFile >> buffer >> goalCount;
	goals = std::vector<POINT>(goalCount);
	for (int i = 0; i < goalCount; i++) {
		dataFile >> goals[i].x >> goals[i].y;
	}

	int n, x, y;
	dataFile >> buffer >> x >> y;
	bob = BobObject({ textureOriginPosition.x + x * 83 + 13, textureOriginPosition.y + y * 83 - 35 }, {x, y});

	dataFile >> buffer >> n;
	while (n--) {
		dataFile >> x >> y;
		holes.push_back(HoleObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}
	
	dataFile >> buffer >> n;
	while (n--) {
		dataFile >> x >> y;
		boxs.push_back(SmallBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}

	dataFile >> buffer >> n;
	while (n--) {
		dataFile >> x >> y;
		boxs.push_back(MediumBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}

	dataFile >> buffer >> n;
	while (n--) {
		dataFile >> x >> y;
		boxs.push_back(LargeBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}
}

bool Level::IsReachGoal() {
	if (bob.IsMoving()) return false;
	if (goals.size() == 0) return false;

	int reachCount = 0;
	for (POINT goal : goals) {
		for (Object& box : boxs) {
			if (isPointInsideObject(box, goal)) reachCount++;
		}
	}
	return reachCount == goals.size();
}

bool Level::IsInsideGameboard(POINT position) {
	return -1 < position.x && position.x < gameboardWidth
		&& -1 < position.y && position.y < gameboardHeight;
}

void Level::MoveUp() {
	moveBuffer.push(UP);
}

void Level::MoveDown() {
	moveBuffer.push(DOWN);
}

void Level::MoveLeft() {
	moveBuffer.push(LEFT);
}

void Level::MoveRight() {
	moveBuffer.push(RIGHT);
}

void Level::Undo() {
	if (undos.empty()) return;

	std::vector<UndoInfo> undoSteps = undos.top();
	undos.pop();

	UndoInfo step;
	for (size_t i = undoSteps.size() - 1; i >= 0; i--) {
		step = undoSteps[i];
		if (step.type == UNDO_UP) {
			findObjectAtPosition(step.objectType, step.objectFinalPosition).MoveDown(step.moveBlock, 0, false);
		} else if (step.type == UNDO_DOWN) {
			findObjectAtPosition(step.objectType, step.objectFinalPosition).MoveUp(step.moveBlock, 0, false);
		} else if (step.type == UNDO_LEFT) {
			findObjectAtPosition(step.objectType, step.objectFinalPosition).MoveRight(step.moveBlock, 0, false);
		} else if (step.type == UNDO_RIGHT) {
			findObjectAtPosition(step.objectType, step.objectFinalPosition).MoveLeft(step.moveBlock, 0, false);
		} else if (step.type == UNDO_FILL) {
			boxs.push_back(filledObjectBuffer.back());
			filledObjectBuffer.pop_back();
			holes.push_back(filledObjectBuffer.back());
			filledObjectBuffer.pop_back();
		}
		if (i == 0) break;
	}
}

void Level::Update() {
	if (moveBuffer.empty()) return;
	if (bob.IsMoving()) return;

	undoBuffer.clear();

	Direction moveDirection = moveBuffer.front();
	moveBuffer.pop();
	if (moveDirection == UP) {
		moveObject(bob, UP, 0);
	} else if (moveDirection == DOWN) {
		moveObject(bob, DOWN, 0);
	} else if (moveDirection == LEFT) {
		moveObject(bob, LEFT, 0);
	} else if (moveDirection == RIGHT) {
		moveObject(bob, RIGHT, 0);
	}
	checkHoleFill();

	if (undoBuffer.size() != 0)
		undos.push(undoBuffer);
}

void Level::Show() {
	background.ShowBitmap();
	for (Object &hole : holes) {
		hole.Show();
	}
	for (size_t i = 1; i < filledObjectBuffer.size(); i += 2) {
		if (filledObjectBuffer[i].IsMoving()) {
			filledObjectBuffer[i - 1].Show();
			filledObjectBuffer[i].Show();
		}
	}
	for (Object &box : boxs) {
		box.Show();
	}
	bob.Show();

	statusDisplay.Show();
	if (IsReachGoal() && !statusDisplay.IsShowing()) {
		statusDisplay.StartClear();
	}
}

bool Level::isPointInsideObject(Object& object, POINT point) {
	Object::Type type = object.GetType();
	POINT objectPosition = object.GetGameboardPosition();
	if (type == Object::HOLE || type == Object::BOB || type == Object::SMALL_BOX) {
		return objectPosition.x == point.x && objectPosition.y == point.y;
	} else if (type == Object::MEDIUM_BOX) {
		return (objectPosition.x == point.x && objectPosition.y == point.y)
			|| (objectPosition.x + 1 == point.x && objectPosition.y == point.y);
	} else if (type == Object::LARGE_BOX) {
		return (objectPosition.x == point.x && objectPosition.y == point.y)
			|| (objectPosition.x + 1 == point.x && objectPosition.y == point.y)
			|| (objectPosition.x == point.x && objectPosition.y + 1 == point.y)
			|| (objectPosition.x + 1 == point.x && objectPosition.y + 1 == point.y);
	}
	return false;
}

bool Level::isObjectOnIce(Object::Type type, POINT position) {
	if (type == Object::BOB || type == Object::SMALL_BOX) {
		return gameboard[position.x][position.y] == 2;
	}
	if (type == Object::MEDIUM_BOX) {
		return gameboard[position.x][position.y] == 2 && gameboard[position.x + 1][position.y] == 2;
	}
	if (type == Object::LARGE_BOX) {
		return gameboard[position.x][position.y] == 2 && gameboard[position.x + 1][position.y] == 2
			&& gameboard[position.x][position.y + 1] == 2 && gameboard[position.x + 1][position.y + 1] == 2;
	}
	throw new runtime_error("Object type wrong!");
}

POINT Level::getBoxRealPosition(POINT origionPosition, POINT gameboardPosition) {
	return { origionPosition.x + gameboardPosition.x * 83 + 6, origionPosition.y + gameboardPosition.y * 82 + 5 };
}

void Level::checkHoleFill() {
	for (size_t holeIndex = 0; holeIndex < holes.size(); holeIndex++) {
		for (size_t boxIndex = 0; boxIndex < boxs.size(); boxIndex++) {
			if (boxs[boxIndex].GetType() != Object::SMALL_BOX) continue;
			POINT holePosition = holes[holeIndex].GetGameboardPosition();
			POINT boxPosition = boxs[boxIndex].GetGameboardPosition();
			if (holePosition.x == boxPosition.x && holePosition.y == boxPosition.y) {
				filledObjectBuffer.push_back(holes[holeIndex]);
				filledObjectBuffer.push_back(boxs[boxIndex]);
				UndoInfo undoInfo;
				undoInfo.type = UNDO_FILL;
				undoBuffer.push_back(undoInfo);

				holes.erase(holes.begin() + (holeIndex--));
				boxs.erase(boxs.begin() + (boxIndex--));
				break;
			}
		}
	}
}

Object& Level::findObjectAtPosition(Object::Type type, POINT position) {
	POINT objectPosition;
	if (type == Object::BOB) {
		objectPosition = bob.GetGameboardPosition();
		if (objectPosition.x == position.x && objectPosition.y == position.y) return bob;
	} else {
		for (Object& box : boxs) {
			if (box.GetType() != type) continue;
			objectPosition = box.GetGameboardPosition();
			if (objectPosition.x == position.x && objectPosition.y == position.y) return box;
		}
	}

	throw runtime_error("can not find object at point(" + to_string(position.x) + ", " + to_string(position.y) + ")");
}

int Level::moveObject(Object &object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	Object::Type type = object.GetType();
	if (type == Object::BOB) {
		moveBlock = getMoveBobBlock(object, direction, waitBlock);
	} else if (type == Object::SMALL_BOX) {
		moveBlock = getMoveSmallBoxBlock(object, direction, waitBlock);
	} else if (type == Object::MEDIUM_BOX) {
		moveBlock = getMoveMediumBoxBlock(object, direction, waitBlock);
	} else if (type == Object::LARGE_BOX) {
		moveBlock = getMoveLargeBoxBlock(object, direction, waitBlock);
	}

	UndoInfo undoInfo = {};
	if (direction == UP) {
		object.MoveUp(moveBlock, waitBlock, true);
		undoInfo.type = UNDO_UP;
	} else if (direction == DOWN) {
		object.MoveDown(moveBlock, waitBlock, true);
		undoInfo.type = UNDO_DOWN;
	} else if (direction == LEFT) {
		object.MoveLeft(moveBlock, waitBlock, true);
		undoInfo.type = UNDO_LEFT;
	} else if (direction == RIGHT) {
		object.MoveRight(moveBlock, waitBlock, true);
		undoInfo.type = UNDO_RIGHT;
	}
	undoInfo.objectType = object.GetType();
	undoInfo.objectFinalPosition = object.GetGameboardPosition();
	undoInfo.moveBlock = moveBlock;
	undoBuffer.push_back(undoInfo);
	return moveBlock;
}

int Level::getMoveBobBlock(Object &object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT position = object.GetGameboardPosition();
	while (!isBobReachWall(position, direction) && (moveBlock == 0 || isObjectOnIce(Object::BOB, position))) {
		position = GetOffsetPoint(position, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

int Level::getMoveSmallBoxBlock(Object &object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT position = object.GetGameboardPosition();
	while (!isSmallBoxReachWall(position, direction) && (moveBlock == 0 || isObjectOnIce(Object::SMALL_BOX, position))) {
		position = GetOffsetPoint(position, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

int Level::getMoveMediumBoxBlock(Object &object, Direction direction, int waitBlock) {
	if (direction == UP || direction == DOWN) return getMoveMediumBoxBlockVertical(object, direction, waitBlock);
	return getMoveMediumBoxBlockHorizontal(object, direction, waitBlock);
}

int Level::getMoveMediumBoxBlockHorizontal(Object& object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT objectMainPosition = object.GetGameboardPosition();
	POINT position = objectMainPosition;
	if (direction == RIGHT) position.x += 1;
	while (!isMediumBoxReachWall(objectMainPosition, direction) && (moveBlock == 0 || isObjectOnIce(Object::MEDIUM_BOX, objectMainPosition))) {
		objectMainPosition = GetOffsetPoint(objectMainPosition, direction);
		position = GetOffsetPoint(position, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

int Level::getMoveMediumBoxBlockVertical(Object& object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT objectMainPosition = object.GetGameboardPosition();
	POINT position1 = objectMainPosition;
	POINT position2 = { objectMainPosition.x + 1, objectMainPosition.y };
	while (!isMediumBoxReachWall(objectMainPosition, direction) && (moveBlock == 0 || isObjectOnIce(Object::MEDIUM_BOX, objectMainPosition))) {
		objectMainPosition = GetOffsetPoint(objectMainPosition, direction);
		position1 = GetOffsetPoint(position1, direction);
		position2 = GetOffsetPoint(position2, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position1) || isPointInsideObject(box, position2)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

int Level::getMoveLargeBoxBlock(Object& object, Direction direction, int waitBlock) {
	if (direction == UP || direction == DOWN) return getMoveLargeBoxBlockVertical(object, direction, waitBlock);
	return getMoveLargeBoxBlockHorizontal(object, direction, waitBlock);
}

int Level::getMoveLargeBoxBlockHorizontal(Object& object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT objectMainPosition = object.GetGameboardPosition();
	POINT position1 = objectMainPosition;
	POINT position2 = { objectMainPosition.x, objectMainPosition.y + 1 };
	if (direction == RIGHT) {
		position1.x += 1;
		position2.x += 1;
	}
	while (!isLargeBoxReachWall(objectMainPosition, direction) && (moveBlock == 0 || isObjectOnIce(Object::LARGE_BOX, objectMainPosition))) {
		objectMainPosition = GetOffsetPoint(objectMainPosition, direction);
		position1 = GetOffsetPoint(position1, direction);
		position2 = GetOffsetPoint(position2, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position1) || isPointInsideObject(box, position2)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

int Level::getMoveLargeBoxBlockVertical(Object& object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	POINT objectMainPosition = object.GetGameboardPosition();
	POINT position1 = objectMainPosition;
	POINT position2 = { objectMainPosition.x + 1, objectMainPosition.y };
	if (direction == DOWN) {
		position1.y += 1;
		position2.y += 1;
	}
	while (!isLargeBoxReachWall(objectMainPosition, direction) && (moveBlock == 0 || isObjectOnIce(Object::LARGE_BOX, objectMainPosition))) {
		objectMainPosition = GetOffsetPoint(objectMainPosition, direction);
		position1 = GetOffsetPoint(position1, direction);
		position2 = GetOffsetPoint(position2, direction);
		for (Object& box : boxs) {
			if (isPointInsideObject(box, position1) || isPointInsideObject(box, position2)) {
				if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
				if (box.IsMoving() || moveObject(box, direction, moveBlock) == 0) return moveBlock;
			}
		}
		moveBlock++;
	}
	return moveBlock;
}

bool Level::isObjectReachWall(Object& object, Direction direction) {
	Object::Type type = object.GetType();
	if (type == Object::BOB) return isBobReachWall(object.GetGameboardPosition(), direction);
	if (type == Object::SMALL_BOX) return isSmallBoxReachWall(object.GetGameboardPosition(), direction);
	throw new runtime_error("Object type wrong!");
}

bool Level::isBobReachWall(POINT position, Direction direction) {
	POINT nextPosition = GetOffsetPoint(position, direction);
	if (!IsInsideGameboard(nextPosition)) return true;
	if (gameboard[nextPosition.x][nextPosition.y] == 0) return true;
	for (Object &hole : holes) {
		POINT holePosition = hole.GetGameboardPosition();
		if (holePosition.x == nextPosition.x && holePosition.y == nextPosition.y) return true;
	}
	return false;
}

bool Level::isSmallBoxReachWall(POINT position, Direction direction) {
	if (direction == UP) {
		return position.y == 0 || gameboard[position.x][position.y - 1] == 0;
	} else if (direction == DOWN) {
		return position.y == gameboardHeight - 1 || gameboard[position.x][position.y + 1] == 0;
	} else if (direction == LEFT) {
		return position.x == 0 || gameboard[position.x - 1][position.y] == 0;
	} else if (direction == RIGHT) {
		return position.x == gameboardWidth - 1 || gameboard[position.x + 1][position.y] == 0;
	}
	throw new runtime_error("Move direction wrong!");
}

bool Level::isMediumBoxReachWall(POINT position, Direction direction) {
	if (direction == UP) {
		return position.y == 0 || gameboard[position.x][position.y - 1] == 0 || gameboard[position.x + 1][position.y - 1] == 0;
	} else if (direction == DOWN) {
		return position.y == gameboardHeight - 1 || gameboard[position.x][position.y + 1] == 0 || gameboard[position.x + 1][position.y + 1] == 0;
	} else if (direction == LEFT) {
		return position.x == 0 || gameboard[position.x - 1][position.y] == 0;
	} else if (direction == RIGHT) {
		return position.x == gameboardWidth - 2 || gameboard[position.x + 2][position.y] == 0;
	}
	throw new runtime_error("Move direction wrong!");
}

bool Level::isLargeBoxReachWall(POINT position, Direction direction) {
	if (direction == UP) {
		return position.y == 0 || gameboard[position.x][position.y - 1] == 0 || gameboard[position.x + 1][position.y - 1] == 0;
	} else if (direction == DOWN) {
		return position.y == gameboardHeight - 2 || gameboard[position.x][position.y + 2] == 0 || gameboard[position.x + 1][position.y + 2] == 0;
	} else if (direction == LEFT) {
		return position.x == 0 || gameboard[position.x - 1][position.y] == 0 || gameboard[position.x - 1][position.y + 1] == 0;
	} else if (direction == RIGHT) {
		return position.x == gameboardWidth - 2 || gameboard[position.x + 2][position.y] == 0 || gameboard[position.x + 2][position.y + 1] == 0;
	}
	throw new runtime_error("Move direction wrong!");
}