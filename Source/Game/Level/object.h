#pragma once

#include "../../Library/gameutil.h"
#include "../util/direction.h"

class Object {
public:
	enum Type {
		TYPE_NONE,
		HOLE,
		BOB,
		SMALL_BOX,
		MEDIUM_BOX,
		LARGE_BOX
	};

	static void Init();
	
	Type GetType();
	POINT GetGameboardPosition();

	bool IsMoving();
	void MoveUp(int block, bool smooth);
	void MoveDown(int block, bool smooth);
	void MoveLeft(int block, bool smooth);
	void MoveRight(int block, bool smooth);

	void Show();

protected:
	static std::string RESOURCES_DIR;
	static game_framework::CMovingBitmap holeTexture;
	static game_framework::CMovingBitmap bobTexture;
	static game_framework::CMovingBitmap smallBoxTexture;
	static game_framework::CMovingBitmap mediumBoxTexture;
	static game_framework::CMovingBitmap largeBoxTexture;
	
	Type objectType = TYPE_NONE;
	game_framework::CMovingBitmap texture;

	POINT gameboardPosition;

private:
	int moveStepCount = 0;
	Direction moveDirection = DIRECTION_NONE;

	int getMoveDistence();
	void updateTexturePosition();
};

class HoleObject: public Object {
public:
	HoleObject(POINT texturePosition, POINT gameboardPosition);
};

class BobObject: public Object {
public:
	BobObject(POINT texturePosition, POINT gameboardPosition);
};

class SmallBoxObject: public Object {
public:
	SmallBoxObject(POINT texturePosition, POINT gameboardPosition);
};

class MediumBoxObject: public Object {
public:
	MediumBoxObject(POINT texturePosition, POINT gameboardPosition);
};

class LargeBoxObject: public Object {
public:
	LargeBoxObject(POINT texturePosition, POINT gameboardPosition);
};