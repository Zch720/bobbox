#include "stdafx.h"
#include "level.h"
#include "../audio_player.h"
#include <string>

std::string Level::RESOURCES_DIR = "./resources/level/";
std::vector<game_framework::CMovingBitmap> Level::levelBackgrounds = std::vector<game_framework::CMovingBitmap>(16);

void Level::Init() {
	Object::Init();
	statusDisplay.Init();
	for (int i = 0; i < 16; i++) {
		levelBackgrounds[i].LoadBitmapByString({ RESOURCES_DIR + "background_" + to_string(i) + ".bmp" });
	}
	backButton.LoadBitmapByString({ RESOURCES_DIR + "back.bmp" });
	backButton.SetTopLeft(20, 20);
	musicButton.LoadBitmapByString({ RESOURCES_DIR + "music_on.bmp", RESOURCES_DIR + "music_off.bmp" });
	musicButton.SetTopLeft(1374, 20);
	soundButton.LoadBitmapByString({ RESOURCES_DIR + "sound_on.bmp", RESOURCES_DIR + "sound_off.bmp" });
	soundButton.SetTopLeft(1445, 20);
}

void Level::Clear() {
	while (!moveBuffer.empty()) moveBuffer.pop();
	while (!undos.empty()) undos.pop();
	isDead = false;
	gameboard.clear();
	blockReachable.clear();
	goals.clear();
	holes.clear();
	boxs.clear();
	statusDisplay.LevelStatusClean();
}

void Level::LoadLevel(int level) {
	background = levelBackgrounds[level - 1];

	std::ifstream dataFile(RESOURCES_DIR + to_string(level - 1) + ".level");

	loadTextureOriginPosition(dataFile);
	loadGameboard(dataFile);
	loadGoals(dataFile);
	loadBob(dataFile);
	loadHoles(dataFile);
	loadBoxs(dataFile);
	dataFile.close();
}

bool Level::IsReachGoal() {
	if (bob.IsMoving()) return false;
	if (goals.size() == 0) return false;

	int reachCount = 0;
	for (Object& box : boxs) {
		if (isBoxOnGoal(box)) reachCount++;
	}
	return reachCount == boxs.size();
}

void Level::SetBackButtonOnClick(Button::OnClickFunc func) {
	backButton.SetOnClickFunc(func);
}

void Level::SetMusicButtonOnClick(Button::OnClickFunc func) {
	musicButton.SetOnClickFunc([this, func]() {
		musicButton.SetFrameIndexOfBitmap(musicButton.GetFrameIndexOfBitmap() ^ 1);
		func();
	});
}
void Level::SetSoundButtonOnClick(Button::OnClickFunc func) {
	soundButton.SetOnClickFunc([this, func]() {
		soundButton.SetFrameIndexOfBitmap(soundButton.GetFrameIndexOfBitmap() ^ 1);
		func();
	});
}

void Level::CheckMouseClick(POINT point) {
	backButton.CheckMouseClick(point);
	musicButton.CheckMouseClick(point);
	soundButton.CheckMouseClick(point);
}

void Level::MoveUp() {
	if (isDead) return;
	moveBuffer.push(UP);
}

void Level::MoveDown() {
	if (isDead) return;
	moveBuffer.push(DOWN);
}

void Level::MoveLeft() {
	if (isDead) return;
	moveBuffer.push(LEFT);
}

void Level::MoveRight() {
	if (isDead) return;
	moveBuffer.push(RIGHT);
}

void Level::Undo() {
	if (undos.empty()) return;

	if (isDead) {
		isDead = false;
		statusDisplay.LevelStatusClean();
	}

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

	AudioPlayer::PlayWalkSound();
}

void Level::Update() {
	if (moveBuffer.empty()) return;
	if (bob.IsMoving()) return;
	if (isDead) {
		while(!moveBuffer.empty()) moveBuffer.pop();
		return;
	}

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
	checkIsDead();

	if (undoBuffer.size() != 0) {
		AudioPlayer::PlayWalkSound();
		undos.push(undoBuffer);
	}
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

	backButton.ShowBitmap();
	musicButton.ShowBitmap();
	soundButton.ShowBitmap();

	statusDisplay.Show();
	if (IsReachGoal() && !statusDisplay.IsShowing()) {
		AudioPlayer::PlayClearSound();
		statusDisplay.StartClear();
	}
	if (isDead && !statusDisplay.IsShowing()) {
		AudioPlayer::PlayDeadSound();
		statusDisplay.StartDead();
	}
}

void Level::loadTextureOriginPosition(std::istream& input) {
	std::string buffer;

	input >> buffer >> textureOriginPosition.x >> textureOriginPosition.y;
}

void Level::loadGameboard(std::istream& input) {
	std::string buffer;

	input >> buffer>> gameboardWidth>> gameboardHeight;

	for (int w = 0; w < gameboardWidth; w++) 
		gameboard.push_back(std::vector<int>(gameboardHeight));
	for (int w = 0; w < gameboardWidth; w++) 
		blockReachable.push_back(std::vector<bool>(gameboardHeight));

	input >> buffer;
	for (int h = 0; h < gameboardHeight; h++) {
		for (int w = 0; w < gameboardWidth; w++) {
			input >> gameboard[w][h];
		}
	}
}

void Level::loadGoals(std::istream& input) {
	std::string buffer;
	int goalCount;

	input >> buffer >> goalCount;
	goals = std::vector<POINT>(goalCount);
	for (int i = 0; i < goalCount; i++) {
		input >> goals[i].x >> goals[i].y;
	}
}

void Level::loadBob(std::istream& input) {
	std::string buffer;
	int x, y;

	input >> buffer >> x >> y;
	bob = BobObject({ textureOriginPosition.x + x * 83 + 13, textureOriginPosition.y + y * 83 - 35 }, {x, y});
}

void Level::loadHoles(std::istream& input) {
	std::string buffer;
	int n, x, y;

	input >> buffer >> n;
	while (n--) {
		input >> x >> y;
		holes.push_back(HoleObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}
}

void Level::loadBoxs(std::istream& input) {
	std::string buffer;
	int n, x, y;

	input >> buffer >> n;
	while (n--) {
		input >> x >> y;
		boxs.push_back(SmallBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}

	input >> buffer >> n;
	while (n--) {
		input >> x >> y;
		boxs.push_back(MediumBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}

	input >> buffer >> n;
	while (n--) {
		input >> x >> y;
		boxs.push_back(LargeBoxObject(getBoxRealPosition(textureOriginPosition, { x, y }), { x, y }));
	}
}

void Level::checkIsDead() {
	findReachableBlock();

	checkedMovableBox.clear();
	for (Object& box : boxs) {
		if (isBoxSideReachable(box, UP)) {
			checkBoxMovable(box, DOWN);
		}
		if (isBoxSideReachable(box, DOWN)) {
			checkBoxMovable(box, UP);
		}
		if (isBoxSideReachable(box, LEFT)) {
			checkBoxMovable(box, RIGHT);
		}
		if (isBoxSideReachable(box, RIGHT)) {
			checkBoxMovable(box, LEFT);
		}
	}

	int reachGoalCount = 0;
	for (Object& box : boxs) {
		if (isBoxOnGoal(box)) reachGoalCount++;
	}

	isDead = (checkedMovableBox.size() == 0) && (reachGoalCount != boxs.size());
}

void Level::findReachableBlock() {
	for (int i = 0; i < gameboardWidth; i++) 
		for (int j = 0; j < gameboardHeight; j++) {
			blockReachable[i][j] = false;
	}

	std::stack<POINT> dfsFindReachable;
	POINT position, offsetPotition;
	dfsFindReachable.push(bob.GetGameboardPosition());
	while (!dfsFindReachable.empty()) {
		position = dfsFindReachable.top();
		dfsFindReachable.pop();

		if (blockReachable[position.x][position.y]) continue;
		blockReachable[position.x][position.y] = true;

		offsetPotition = GetOffsetPoint(position, UP);
		if (isPointInsideGameboard(offsetPotition) && gameboard[offsetPotition.x][offsetPotition.y] != 0 && isBlockEmpty(offsetPotition)) {
			dfsFindReachable.push(offsetPotition);
		}
		offsetPotition = GetOffsetPoint(position, DOWN);
		if (isPointInsideGameboard(offsetPotition) && gameboard[offsetPotition.x][offsetPotition.y] != 0 && isBlockEmpty(offsetPotition)) {
			dfsFindReachable.push(offsetPotition);
		}
		offsetPotition = GetOffsetPoint(position, LEFT);
		if (isPointInsideGameboard(offsetPotition) && gameboard[offsetPotition.x][offsetPotition.y] != 0 && isBlockEmpty(offsetPotition)) {
			dfsFindReachable.push(offsetPotition);
		}
		offsetPotition = GetOffsetPoint(position, RIGHT);
		if (isPointInsideGameboard(offsetPotition) && gameboard[offsetPotition.x][offsetPotition.y] != 0 && isBlockEmpty(offsetPotition)) {
			dfsFindReachable.push(offsetPotition);
		}
	}
}

bool Level::checkBoxMovable(Object& object, Direction direction) {
	if (isBoxAlreadyMoveable(object, direction)) return true;
	if (isObjectReachWall(object.GetType(), object.GetGameboardPosition(), direction)) return false;

	std::vector<POINT> boxSidePositions = getObjectSidePositions(object.GetType(), object.GetGameboardPosition(), direction);
	for (Object& box : boxs) {
		for (POINT sidePosition : boxSidePositions) {
			if (isPointInsideObject(box, sidePosition)) {
				if (!checkBoxMovable(box, direction)) return false;
			}
		}
	}

	int infoIndex = getBoxIndexInMoveableInfos(object);
	if (direction == UP) checkedMovableBox[infoIndex].up = true;
	if (direction == DOWN) checkedMovableBox[infoIndex].down = true;
	if (direction == LEFT) checkedMovableBox[infoIndex].left = true;
	if (direction == RIGHT) checkedMovableBox[infoIndex].right = true;
	return true;
}

bool Level::isBoxAlreadyMoveable(Object& box, Direction direction) {
	for (MoveableInfo moveableInfo : checkedMovableBox) {
		POINT boxPosition = box.GetGameboardPosition();
		if (moveableInfo.position.x == boxPosition.x && moveableInfo.position.y == boxPosition.y) {
			if (direction == UP && moveableInfo.up) return true;
			if (direction == DOWN && moveableInfo.down) return true;
			if (direction == LEFT && moveableInfo.left) return true;
			if (direction == RIGHT && moveableInfo.right) return true;
		}
	}
	return false;
}

int Level::getBoxIndexInMoveableInfos(Object &box) {
	for (size_t i = 0; i < checkedMovableBox.size(); i++) {
		POINT boxPosition = box.GetGameboardPosition();
		if (checkedMovableBox[i].position.x == boxPosition.x && checkedMovableBox[i].position.y == boxPosition.y) {
			return i;
		}
	}
	checkedMovableBox.push_back({ box.GetGameboardPosition(), false, false, false, false });
	return checkedMovableBox.size() - 1;
}

bool Level::isPointInsideGameboard(POINT position) {
	return -1 < position.x && position.x < gameboardWidth
		&& -1 < position.y && position.y < gameboardHeight;
}

bool Level::isBoxSideReachable(Object& box, Direction direction) {
	std::vector<POINT> sidePositions = getObjectSidePositions(box.GetType(), box.GetGameboardPosition(), direction);
	for (POINT position : sidePositions) {
		if (isBlockReachable(position)) return true;
	}
	return false;
}

bool Level::isBlockReachable(POINT position) {
	return isPointInsideGameboard(position) && blockReachable[position.x][position.y];
}

bool Level::isBlockEmpty(POINT position) {
	bool result = false;
	for (Object &box : boxs) {
		result |= isPointInsideObject(box, position);
	}
	for (Object &hole : holes) {
		result |= isPointInsideObject(hole, position);
	}
	return !result;
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

bool Level::isBoxOnGoal(Object& box) {
	int count = 0;
	for (POINT goal : goals) {
		if (isPointInsideObject(box, goal)) count++;
	}
	if (box.GetType() == Object::SMALL_BOX) return count == 1;
	if (box.GetType() == Object::MEDIUM_BOX) return count == 2;
	if (box.GetType() == Object::LARGE_BOX) return count == 4;
	throw new runtime_error("Object type wrong!");
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
				UndoInfo undoInfo = {};
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

std::vector<POINT> Level::getObjectSidePositions(Object::Type type, POINT objectPosition, Direction direction) {
	std::vector<POINT> result;
	if (type == Object::BOB) {
		result.push_back(GetOffsetPoint(objectPosition, direction));
	}
	if (type == Object::SMALL_BOX) {
		result.push_back(GetOffsetPoint(objectPosition, direction));
	} else if (type == Object::MEDIUM_BOX) {
		if (direction == LEFT) {
			result.push_back(GetOffsetPoint(objectPosition, direction));
		} else if (direction == RIGHT) {
			result.push_back(GetOffsetPoint(GetOffsetPoint(objectPosition, RIGHT), direction));
		} else {
			result.push_back(GetOffsetPoint(objectPosition, direction));
			result.push_back(GetOffsetPoint(GetOffsetPoint(objectPosition, RIGHT), direction));
		}
	} else if (type == Object::LARGE_BOX) {
		POINT position1 = objectPosition;
		POINT position2;
		if (direction == LEFT) {
			position2 = GetOffsetPoint(position1, DOWN);
		} else if (direction == RIGHT) {
			position1 = GetOffsetPoint(position1, RIGHT);
			position2 = GetOffsetPoint(position1, DOWN);
		} else if (direction == UP) {
			position2 = GetOffsetPoint(position1, RIGHT);
		} else if (direction == DOWN) {
			position1 = GetOffsetPoint(position1, DOWN);
			position2 = GetOffsetPoint(position1, RIGHT);
		}
		position1 = GetOffsetPoint(position1, direction);
		position2 = GetOffsetPoint(position2, direction);
		result.push_back(position1);
		result.push_back(position2);
	}
	return result;
}

int Level::moveObject(Object& object, Direction direction, int waitBlock) {
	int moveBlock = getObjectMoveBlock(object, direction, waitBlock);

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
	if (moveBlock != 0) undoBuffer.push_back(undoInfo);
	return moveBlock;
}

int Level::getObjectMoveBlock(Object& object, Direction direction, int waitBlock) {
	int moveBlock = 0;
	Object::Type objectType = object.GetType();
	POINT objectPosition = object.GetGameboardPosition();
	std::vector<POINT> sidePositions;
	while (!isObjectReachWall(objectType, objectPosition, direction) && (moveBlock == 0 || isObjectOnIce(objectType, objectPosition))) {
		sidePositions = getObjectSidePositions(objectType, objectPosition, direction);
		for (Object& box : boxs) {
			for (POINT position : sidePositions) {
				if (isPointInsideObject(box, position)) {
					if (moveBlock != 0 && !isObjectOnIce(box.GetType(), box.GetGameboardPosition())) return moveBlock;
					if (box.IsMoving() || moveObject(box, direction, moveBlock + waitBlock) == 0) return moveBlock;
				}
			}
		}
		objectPosition = GetOffsetPoint(objectPosition, direction);
		moveBlock++;
	}
	return moveBlock;
}

bool Level::isObjectReachWall(Object::Type type, POINT position, Direction direction) {
	std::vector<POINT> sidePositions = getObjectSidePositions(type, position, direction);
	for (POINT position : sidePositions) {
		if (!isPointInsideGameboard(position) || gameboard[position.x][position.y] == 0) return true;
	}
	if (type == Object::BOB) return isPointOnHoles(GetOffsetPoint(position, direction));
	return false;
}

bool Level::isPointOnHoles(POINT position) {
	for (Object &hole : holes) {
		POINT holePosition = hole.GetGameboardPosition();
		if (holePosition.x == position.x && holePosition.y == position.y) return true;
	}
	return false;
}