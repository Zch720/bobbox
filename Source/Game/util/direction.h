#pragma once

enum Direction {
	DIRECTION_NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

POINT GetDirectionPoint(Direction direction);
POINT GetOffsetPoint(POINT point, Direction direction);