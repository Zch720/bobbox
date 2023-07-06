#include "stdafx.h"
#include "level.h"
#include <string>
#include <fstream>

std::string Level::RESOURCES_DIR = "./resources/level/";
std::vector<game_framework::CMovingBitmap> Level::levelBackgrounds = std::vector<game_framework::CMovingBitmap>(16);

void Level::Init() {
	Object::Init();
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
			if (isPointInsizObject(box, goal)) reachCount++;
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

void Level::Update() {
	if (moveBuffer.empty()) return;
	if (bob.IsMoving()) return;

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
}

void Level::Show() {
	background.ShowBitmap();
	for (Object &box : boxs) {
		box.Show();
	}
	bob.Show();
}

bool Level::isPointInsizObject(Object& object, POINT point) {
	Object::Type type = object.GetType();
	POINT objectPosition = object.GetGameboardPosition();
	if (type == Object::HOLE || type == Object::BOB || type == Object::SMALL_BOX) {
		return objectPosition.x == point.x && objectPosition.y == point.y;
	} else if (type == Object::MEDIUM_BOX) {
		return (objectPosition.x == point.x && objectPosition.y == point.y)
			|| (objectPosition.x == point.x + 1 && objectPosition.y == point.y);
	} else if (type == Object::LARGE_BOX) {
		return (objectPosition.x == point.x && objectPosition.y == point.y)
			|| (objectPosition.x == point.x + 1 && objectPosition.y == point.y)
			|| (objectPosition.x == point.x && objectPosition.y == point.y + 1)
			|| (objectPosition.x == point.x + 1 && objectPosition.y == point.y + 1);
	}
	return false;
}

POINT Level::getBoxRealPosition(POINT origionPosition, POINT gameboardPosition) {
	return { origionPosition.x + gameboardPosition.x * 83 + 6, origionPosition.y + gameboardPosition.y * 82 + 5 };
}

int Level::moveObject(Object &object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	if (object.GetType() == Object::BOB) {
		moveBlock = getMoveBobBlock(object, direction, waitBlock);
	} else if (object.GetType() == Object::SMALL_BOX) {
		moveBlock = getMoveSmallBoxBlock(object, direction, waitBlock);
	}

	if (direction == UP) {
		object.MoveUp(moveBlock, waitBlock, true);
	} else if (direction == DOWN) {
		object.MoveDown(moveBlock, waitBlock, true);
	} else if (direction == LEFT) {
		object.MoveLeft(moveBlock, waitBlock, true);
	} else if (direction == RIGHT) {
		object.MoveRight(moveBlock, waitBlock, true);
	}
	return moveBlock;
}

int Level::getMoveBobBlock(Object &object, Direction direction, int waitBlock) {
	bool reachBox = false;
	int moveCount = 0;
	POINT position = object.GetGameboardPosition();
	while (!reachBox && !isBobReachWall(position, direction) && (moveCount == 0 || gameboard[position.x][position.y] == 2)) {
		position = GetOffsetPoint(position, direction);
		for (Object &box : boxs) {
			if (isPointInsizObject(box, position)) {
				reachBox = true;
				moveCount += moveObject(box, direction, moveCount + waitBlock);
				break;
			}
		}
		if (reachBox) break;
		moveCount++;
	}
	return moveCount;
}

int Level::getMoveSmallBoxBlock(Object &object, Direction direction, int waitBlock) {
	bool reachBox = false;
	int moveCount = 0;
	POINT position = object.GetGameboardPosition();
	while (!reachBox && !isSmallBoxReachWall(position, direction) && (moveCount == 0 || gameboard[position.x][position.y] == 2)) {
		position = GetOffsetPoint(position, direction);
		for (Object& box : boxs) {
			if (isPointInsizObject(box, position)) {
				reachBox = true;
				moveCount += moveObject(box, direction, moveCount + waitBlock);
				break;
			}
		}
		if (reachBox) break;
		moveCount++;
	}
	return moveCount;
}

bool Level::isObjectReachWall(Object& object, Direction direction) {
	Object::Type type = object.GetType();
	if (type == Object::BOB) return isBobReachWall(object.GetGameboardPosition(), direction);
	if (type == Object::SMALL_BOX) return isSmallBoxReachWall(object.GetGameboardPosition(), direction);
	throw new runtime_error("Object type wrong!");
}

bool Level::isBobReachWall(POINT position, Direction direction) {
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