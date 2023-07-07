#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"

using namespace game_framework;

CGameStateRun::CGameStateRun(CGame* g) : CGameState(g) {
}

CGameStateRun::~CGameStateRun() {
}

void CGameStateRun::OnBeginState() {
	if (currentLevel == 0 || currentLevel == 17) {
		gameType = STORY;
	} else {
		gameType = LEVEL;
		levelManager.LoadLevel(currentLevel);
	}

	if (lastLevel < 1) {
		SwitchScene::OpenLeft();
	} else {
		SwitchScene::OpenRight();
	}

	lastLevel = currentLevel;
}

void CGameStateRun::OnInit() {
	levelManager.Init();
}

void CGameStateRun::OnMove() {
	levelManager.Update();
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (isSwitchingScene) return;

	if (levelManager.IsReachGoal()) {
		if (nChar == VK_RETURN && !SwitchScene::IsClosing()) {
			isSwitchingScene = true;
			SwitchScene::CloseRight();
		}
		return;
	}

	if (nChar == VK_ESCAPE) {
		isSwitchingScene = true;
		SwitchScene::CloseRight();
	} else if (nChar == VK_UP || nChar == 'W') {
		levelManager.MoveUp();
	} else if (nChar == VK_DOWN || nChar == 'S') {
		levelManager.MoveDown();
	} else if (nChar == VK_LEFT || nChar == 'A') {
		levelManager.MoveLeft();
	} else if (nChar == VK_RIGHT || nChar == 'D') {
		levelManager.MoveRight();
	} else if (nChar == 'Z' || nChar == 'U') {
		levelManager.Undo();
	} else if (nChar == 'R') {
		levelManager.Clear();
		GotoGameState(GAME_STATE_RUN);
	}
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

void CGameStateRun::OnLButtonDown(UINT nFlags, CPoint point) {
}

void CGameStateRun::OnLButtonUp(UINT nFlags, CPoint point) {
}

void CGameStateRun::OnMouseMove(UINT nFlags, CPoint point) {
}

void CGameStateRun::OnRButtonDown(UINT nFlags, CPoint point) {
}

void CGameStateRun::OnRButtonUp(UINT nFlags, CPoint point) {
}

void CGameStateRun::OnShow() {
	if (gameType == LEVEL) {
		levelManager.Show();
	}

	SwitchScene::Show();
	if (isSwitchingScene && !SwitchScene::IsClosing()) {
		bool reachGoal = levelManager.IsReachGoal();
		levelManager.Clear();
		isSwitchingScene = false;
		if (reachGoal) {
			currentLevel++;
			GotoGameState(GAME_STATE_RUN);
		} else {
			GotoGameState(GAME_STATE_INIT);
		}
	}
}
