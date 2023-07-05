#include "stdafx.h"
#include "direction.h"

POINT GetDirectionPoint(Direction direction) {
	if (direction == UP) return { 0, -1 };
	if (direction == DOWN) return { 0, 1 };
	if (direction == LEFT) return { -1, 0 };
	if (direction == RIGHT) return { 1, 0 };
	return { 0, 0 };
}

POINT GetOffsetPoint(POINT point, Direction direction) {
	POINT directionPoint = GetDirectionPoint(direction);
	return { point.x + directionPoint.x, point.y + directionPoint.y };
}