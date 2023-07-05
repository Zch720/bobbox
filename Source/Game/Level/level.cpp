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

void Level::LoadLevel(int level) {
	background = levelBackgrounds[level - 1];

	std::ifstream dataFile(RESOURCES_DIR + to_string(level - 1) + ".level");
	POINT textureOriginPosition{0, 0};
	std::string buffer;

	dataFile >> buffer>> gameboardWidth>> gameboardHeight;
	dataFile >> buffer >> textureOriginPosition.x >> textureOriginPosition.y;

	gameboard.clear();
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
	goals.clear();
	goals = std::vector<POINT>(goalCount);
	for (int i = 0; i < goalCount; i++) {
		dataFile >> goals[i].x >> goals[i].y;
	}

	int x, y;
	dataFile >> buffer >> x >> y;
	bob = BobObject({ textureOriginPosition.x + x * 83 + 13, textureOriginPosition.y + y * 83 - 35 }, {x, y});

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
		moveObject(bob, UP);
	} else if (moveDirection == DOWN) {
		moveObject(bob, DOWN);
	} else if (moveDirection == LEFT) {
		moveObject(bob, LEFT);
	} else if (moveDirection == RIGHT) {
		moveObject(bob, RIGHT);
	}
}

void Level::Show() {
	background.ShowBitmap();
	bob.Show();
}

void Level::moveObject(Object &object, Direction direction) {
	int moveBlock = 0;
	if (object.GetType() == Object::BOB) {
		moveBlock = getMoveBobBlock(object, direction);
	}

	if (direction == UP) {
		object.MoveUp(moveBlock, true);
	} else if (direction == DOWN) {
		object.MoveDown(moveBlock, true);
	} else if (direction == LEFT) {
		object.MoveLeft(moveBlock, true);
	} else if (direction == RIGHT) {
		object.MoveRight(moveBlock, true);
	}
}

int Level::getMoveBobBlock(Object &object, Direction direction) {
	int moveCount = 0;
	POINT position = object.GetGameboardPosition();
	while (!isBobReachWall(position, direction) && (moveCount == 0 || gameboard[position.x][position.y] == 2)) {
		moveCount++;
		position = GetOffsetPoint(position, direction);
	}
	return moveCount;
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
	return true;
}