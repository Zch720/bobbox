#include "stdafx.h"
#include "object.h"

std::string Object::RESOURCES_DIR = "./resources/level/";
game_framework::CMovingBitmap Object::holeTexture;
game_framework::CMovingBitmap Object::bobTexture;
game_framework::CMovingBitmap Object::smallBoxTexture;
game_framework::CMovingBitmap Object::mediumBoxTexture;
game_framework::CMovingBitmap Object::largeBoxTexture;

void Object::Init() {
	holeTexture.LoadBitmapByString({ RESOURCES_DIR + "hole.bmp" });
	smallBoxTexture.LoadBitmapByString({ RESOURCES_DIR + "box_s.bmp" });
	mediumBoxTexture.LoadBitmapByString({ RESOURCES_DIR + "box_m.bmp" });
	largeBoxTexture.LoadBitmapByString({ RESOURCES_DIR + "box_l.bmp" });
	bobTexture.LoadBitmapByString({
		RESOURCES_DIR + "bob1.bmp",
		RESOURCES_DIR + "bob2.bmp",
		RESOURCES_DIR + "bob3.bmp",
	}, RGB(0x00, 0x00, 0xff));
	bobTexture.SetAnimation(150, false);
}

Object::Type Object::GetType() {
	return objectType;
}

POINT Object::GetGameboardPosition() {
	return gameboardPosition;
}

bool Object::IsMoving() {
	return moveStepCount != 0;
}

void Object::MoveUp(int block, bool smooth) {
	moveStepCount = 6 * block;
	moveDirection = UP;
	gameboardPosition.y -= block;

	if (!smooth) {
		while(moveStepCount) updateTexturePosition();
	}
}

void Object::MoveDown(int block, bool smooth) {
	moveStepCount = 6 * block;
	moveDirection = DOWN;
	gameboardPosition.y += block;

	if (!smooth) {
		while(moveStepCount) updateTexturePosition();
	}
}

void Object::MoveLeft(int block, bool smooth) {
	moveStepCount = 6 * block;
	moveDirection = LEFT;
	gameboardPosition.x -= block;

	if (!smooth) {
		while(moveStepCount) updateTexturePosition();
	}
}

void Object::MoveRight(int block, bool smooth) {
	moveStepCount = 6 * block;
	moveDirection = RIGHT;
	gameboardPosition.x += block;

	if (!smooth) {
		while(moveStepCount) updateTexturePosition();
	}
}

void Object::Show() {
	updateTexturePosition();
	texture.ShowBitmap();
}

int Object::getMoveDistence() {
	if (moveStepCount % 6 == 0) return 13;
	return 14;
}

void Object::updateTexturePosition() {
	if (moveStepCount == 0) return;

	int left = texture.GetLeft();
	int top = texture.GetTop();
	if (moveDirection == UP) {
		top -= getMoveDistence();
	} else if (moveDirection == DOWN) {
		top += getMoveDistence();
	} else if (moveDirection == LEFT) {
		left -= getMoveDistence();
	} else if (moveDirection == RIGHT) {
		left += getMoveDistence();
	}
	texture.SetTopLeft(left, top);
	moveStepCount--;
}


HoleObject::HoleObject(POINT texturePosition, POINT gameboardPosition) : Object() {
	objectType = HOLE;
	texture = holeTexture;
	texture.SetTopLeft(texturePosition.x, texturePosition.y);
	this -> gameboardPosition = gameboardPosition;
}

BobObject::BobObject(POINT texturePosition, POINT gameboardPosition) : Object() {
	objectType = BOB;
	texture = bobTexture;
	texture.SetTopLeft(texturePosition.x, texturePosition.y);
	this -> gameboardPosition = gameboardPosition;
}

SmallBoxObject::SmallBoxObject(POINT texturePosition, POINT gameboardPosition) : Object() {
	objectType = SMALL_BOX;
	texture = smallBoxTexture;
	texture.SetTopLeft(texturePosition.x, texturePosition.y);
	this -> gameboardPosition = gameboardPosition;
}

MediumBoxObject::MediumBoxObject(POINT texturePosition, POINT gameboardPosition) : Object() {
	objectType = MEDIUM_BOX;
	texture = mediumBoxTexture;
	texture.SetTopLeft(texturePosition.x, texturePosition.y);
	this -> gameboardPosition = gameboardPosition;
}

LargeBoxObject::LargeBoxObject(POINT texturePosition, POINT gameboardPosition) : Object() {
	objectType = LARGE_BOX;
	texture = largeBoxTexture;
	texture.SetTopLeft(texturePosition.x, texturePosition.y);
	this -> gameboardPosition = gameboardPosition;
}