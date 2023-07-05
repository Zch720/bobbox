#pragma once

#include <functional>
#include "../../Library/gameutil.h"

class Button: public game_framework::CMovingBitmap {
public:
	typedef std::function<void()> OnClickFunc;

	bool IsHover(POINT point);

	void SetOnClickFunc(OnClickFunc func);

	void CheckMouseClick(POINT point);
	void Click();

private:
	OnClickFunc onClickFunc = nullptr;
};