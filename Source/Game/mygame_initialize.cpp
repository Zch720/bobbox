#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"

using namespace game_framework;

CGameStateInit::CGameStateInit(CGame *g) : CGameState(g) {
}

void CGameStateInit::OnInit() {
	ShowInitProgress(0, "Start Initialize...");
}

void CGameStateInit::OnBeginState() {
}

void CGameStateInit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

void CGameStateInit::OnLButtonDown(UINT nFlags, CPoint point) {
	GotoGameState(GAME_STATE_OVER);
}

void CGameStateInit::OnShow() {
}
