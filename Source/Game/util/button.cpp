#include "stdafx.h"
#include "button.h"

bool Button::IsHover(POINT point) {
	int left = GetLeft();
	int right = left + GetWidth();
	int top = GetTop();
	int bottom = top + GetHeight();
	return left <= point.x && point.x <= right
		&& top <= point.y && point.y <= bottom;
}

void Button::SetOnClickFunc(OnClickFunc func) {
	onClickFunc = func;
}

void Button::CheckMouseClick(POINT point) {
	if (IsHover(point)) Click();
}

void Button::Click() {
	if (onClickFunc != nullptr) {
		onClickFunc();
	}
}