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
	if (currentLevel == 0) {
		gameType = STORY;
		storyManager.StartStoryBegin();
	} else if (currentLevel == 17) {
		gameType = STORY;
		storyManager.StartStoryEnd();
	} else {
		gameType = LEVEL;
		levelManager.LoadLevel(currentLevel);
	}

	if (lastLevel == 0 || lastLevel == -1) {
		SwitchScene::OpenLeft();
	} else if (lastLevel != -2) {
		SwitchScene::OpenRight();
	}

	lastLevel = currentLevel;
}

void CGameStateRun::OnInit() {
	levelManager.Init();
	levelManager.SetBackButtonOnClick([this]() {
		isSwitchingScene = true;
		AudioPlayer::PlayTransitionSound();
		SwitchScene::CloseRight();
	});
	levelManager.SetMusicButtonOnClick([]() {
		AudioPlayer::SwitchMusicOnOff();
		if (AudioPlayer::GetMusicOnOff()) {
			AudioPlayer::PlayBGM();
		} else {
			AudioPlayer::StopBGM();
		}
	});
	levelManager.SetSoundButtonOnClick([]() {
		AudioPlayer::SwitchSoundOnOff();
	});
}

void CGameStateRun::OnMove() {
	if (gameType == LEVEL) {
		levelManager.Update();
	}
}

void CGameStateRun::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (gameType == STORY) {
		storyKeyInput(nChar);
	} else if (gameType == LEVEL) {
		levelKeyInput(nChar);
	}
}

void CGameStateRun::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

void CGameStateRun::OnLButtonDown(UINT nFlags, CPoint point) {
	levelManager.CheckMouseClick(point);
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
	if (gameType == STORY) {
		storyShow();
	} else if (gameType == LEVEL) {
		levelShow();
	}
}

void CGameStateRun::storyKeyInput(char nChar) {
	if (nChar == VK_SPACE) {
		if (currentLevel == 0) {
			currentLevel++;
			GotoGameState(GAME_STATE_RUN);
		} else {
			GotoGameState(GAME_STATE_INIT);
		}
	} else if (nChar == VK_ESCAPE) {
		GotoGameState(GAME_STATE_INIT);
	}
}

void CGameStateRun::levelKeyInput(char nChar) {
	if (isSwitchingScene) return;

	if (levelManager.IsReachGoal()) {
		if (nChar == VK_RETURN && !SwitchScene::IsClosing()) {
			isSwitchingScene = true;
			AudioPlayer::PlayTransitionSound();
			SwitchScene::CloseRight();
		}
		return;
	}

	if (nChar == VK_ESCAPE) {
		isSwitchingScene = true;
		AudioPlayer::PlayTransitionSound();
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
		lastLevel = -2;
		levelManager.Clear();
		GotoGameState(GAME_STATE_RUN);
	}
}

void  CGameStateRun::storyShow() {
	if (!storyManager.IsShowing()) return;

	CDC *pDC = CDDraw::GetBackCDC();
	storyManager.ShowText(pDC);
	CDDraw::ReleaseBackCDC();
	
	if (!storyManager.IsShowing()) {
		if (currentLevel == 0) {
			currentLevel++;
			GotoGameState(GAME_STATE_RUN);
		} else {
			GotoGameState(GAME_STATE_INIT);
		}
	}
}

void  CGameStateRun::levelShow() {
	levelManager.Show();

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