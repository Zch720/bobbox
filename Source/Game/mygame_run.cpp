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
}

void CGameStateRun::OnMove() {
}

void CGameStateRun::OnInit() {
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
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
}
