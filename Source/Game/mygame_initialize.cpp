#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"

using namespace game_framework;

int currentLevel = -1;
int lastLevel = -1;

CGameStateInit::CGameStateInit(CGame *g) : CGameState(g) {
}

void CGameStateInit::OnInit() {
	ShowInitProgress(0, "Start Initialize...");
	SwitchScene::Init();
	startLayout.Init();
	chooseLevelLayout.Init();
	chooseLevelLayout.SetBackButtonOnClickFunc([this]() {
		startLayoutShown = false;
	});
}

void CGameStateInit::OnBeginState() {
	SwitchScene::OpenLeft();
	lastLevel = -1;
}

void CGameStateInit::OnMove() {
}

void CGameStateInit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (isSwitchingScene) return;
	if (!startLayoutShown) {
		startLayoutShown = true;
		chooseLevelLayout.Reset();
	} else {
		if (nChar == VK_ESCAPE) {
			startLayoutShown = false;
		} else if (nChar == VK_LEFT || nChar == 'A') {
			chooseLevelLayout.ChooserMoveLeft();
		} else if (nChar == VK_RIGHT || nChar == 'D') {
			chooseLevelLayout.ChooserMoveRight();
		} else if (nChar == VK_UP || nChar == 'W') {
			chooseLevelLayout.ChooserMoveUp();
		} else if (nChar == VK_DOWN || nChar == 'S') {
			chooseLevelLayout.ChooserMoveDown();
		} else if (nChar == VK_RETURN) {
			currentLevel = chooseLevelLayout.ChooseLevel();
			if (currentLevel != -1) {
				isSwitchingScene = true;
				SwitchScene::CloseRight();
				//GotoGameState(GAME_STATE_RUN);
			}
		}
	}
}

void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point) {
	if (isSwitchingScene) return;
	if (!startLayoutShown) {
		startLayoutShown = true;
		chooseLevelLayout.Reset();
	} else {
		currentLevel = chooseLevelLayout.GetMouseClickLevelButton(point);
		if (currentLevel != -1) {
			isSwitchingScene = true;
			SwitchScene::CloseRight();
			//GotoGameState(GAME_STATE_RUN);
		}
	}
}

void CGameStateInit::OnMouseMove(UINT nFlags, CPoint point) {
	if (startLayoutShown) {
		chooseLevelLayout.MouseMove(point);
	}
}

void CGameStateInit::OnShow() {
	if (startLayoutShown) {
		chooseLevelLayout.Show();
	} else {
		startLayout.Show();
	}
	
	SwitchScene::Show();
	if (isSwitchingScene) {
		if (!SwitchScene::IsClosing()) {
			isSwitchingScene = false;
			GotoGameState(GAME_STATE_RUN);
		}
	}
}
