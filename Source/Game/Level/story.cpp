#include "stdafx.h"
#include "story.h"
#include "../../Library/gameutil.h"

const std::vector<std::string> Story::beginStoryLines = {
	"YOU ARE BOB.",
	"YOU WERE HIRED TO PUSH BOXES",
	"THIS IS YOUR DUTY",
	"TO PUSH BOXES.",
	"HOT OR COLD",
	"YOU MUST PUSH BOXES."
};
const std::vector<std::string> Story::endStoryLines = {
	"YOU DID IT BOB!",
	"YOU PUSHED ALL THE BOXES.",
	"YOU ARE EMEPLOYEE OF THE MONTH!"
};

void Story::StartStoryBegin() {
	isShowing = true;
	nextTextCount = 0;
	showingLine = 0;
	showingStory = beginStoryLines;
	showingString = "";
}

void Story::StartStoryEnd() {
	isShowing = true;
	nextTextCount = 0;
	showingLine = 0;
	showingStory = endStoryLines;
	showingString = "";
}

bool Story::IsShowing() {
	return isShowing;
}

void Story::ShowText(CDC* pDC) {
	showText(pDC);

	if (showingString.size() == showingStory[showingLine].size()) {
		if (clock() - waitNextString < WAIT_AFTER_SHOW_STRING) return;
		if (++showingLine == showingStory.size()) {
			isShowing = false;
			return;
		}
		showingString = "";
	}

	if (nextTextCount == 0) {
		showingString += showingStory[showingLine][showingString.size()];
	}
	nextTextCount = (nextTextCount + 1) & 0b1;

	if (showingString.size() == showingStory[showingLine].size()) {
		waitNextString = clock();
	}
}

void Story::showText(CDC* pDC) {
	game_framework::CTextDraw::ChangeFontLog(pDC, 27, "Press Start 2P", RGB(0xff, 0xff, 0xff));
	SIZE textSize = calculateShowingStringSize(pDC);
	game_framework::CTextDraw::Print(pDC, 768 - textSize.cx / 2, 356 - textSize.cy / 2, showingString);
}

SIZE Story::calculateShowingStringSize(CDC* pDC) {
	return pDC -> GetOutputTextExtent(showingString.c_str());
}