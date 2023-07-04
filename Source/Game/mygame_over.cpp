#include "stdafx.h"
#include "../Core/Resource.h"
#include <mmsystem.h>
#include <ddraw.h>
#include "../Library/audio.h"
#include "../Library/gameutil.h"
#include "../Library/gamecore.h"
#include "mygame.h"

using namespace game_framework;

CGameStateOver::CGameStateOver(CGame *g): CGameState(g) {
}

void CGameStateOver::OnMove() {
}

void CGameStateOver::OnBeginState() {
}

void CGameStateOver::OnInit() {
	ShowInitProgress(66, "Initialize...");
	ShowInitProgress(100, "OK!");
}

void CGameStateOver::OnShow() {
	exit(0);
}
