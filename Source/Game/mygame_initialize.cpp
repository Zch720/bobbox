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

CGameStateInit::CGameStateInit(CGame *g) : CGameState(g) {
}

void CGameStateInit::OnInit() {
	ShowInitProgress(0, "Start Initialize...");
	startLayout.Init();
	chooseLevelLayout.Init();
	chooseLevelLayout.SetBackButtonOnClickFunc([this]() {
		startLayoutShown = false;
	});
}

void CGameStateInit::OnBeginState() {
}

void CGameStateInit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
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
		} else {
			GotoGameState(GAME_STATE_OVER);
		}
	}
}

void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point) {
	if (!startLayoutShown) {
		startLayoutShown = true;
		chooseLevelLayout.Reset();
	} else {
		chooseLevelLayout.GetMouseClickLevelButton(point);
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
}
