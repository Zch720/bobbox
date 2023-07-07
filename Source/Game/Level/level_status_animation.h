#pragma

#include "../../Library/gameutil.h"

class LevelStatusDisplay {
public:
	void Init();
	void LevelStatusClean();
	
	void StartClear();
	void StartDead();

	bool IsShowing();
	void Show();

private:
	static const std::string RESOURCES_DIR;
	static const int MAX_FRAME_COUNT = 13;

	enum StatusType {
		STATE_NONE,
		CLEAR,
		DEAD,
	};

	game_framework::CMovingBitmap clear;
	game_framework::CMovingBitmap dead;
	StatusType currentStatus = STATE_NONE;
};