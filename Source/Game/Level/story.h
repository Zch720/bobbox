#pragma once

#include <ctime>
#include <string>
#include <vector>

class Story {
public:
	void StartStoryBegin();
	void StartStoryEnd();

	bool IsShowing();
	void ShowText(CDC *pDC);

private:
	static const clock_t WAIT_AFTER_SHOW_STRING = 1000;
	static const std::vector<std::string> beginStoryLines;
	static const std::vector<std::string> endStoryLines;

	bool isShowing = false;
	clock_t waitNextString;
	int nextTextCount;
	int showingLine;
	std::string showingString;
	std::vector<std::string> showingStory;

	void showText(CDC *pDC);
	SIZE calculateShowingStringSize(CDC *pDC);
};