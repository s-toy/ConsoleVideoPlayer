#include "ConsoleVideoPlayer.h"
#include "Common.h"
#include <windows.h>
#include <chrono>
#include <ratio>
#include <thread>
#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace cvplayer;

//******************************************************************************************************
//FUNCTION:
static void __printVideoFrame(cv::VideoCapture& vVideoCapture)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, cursorPos);

	cv::Mat frame;
	vVideoCapture >> frame;

	cv::cvtColor(frame, frame, CV_RGB2GRAY);
	cv::resize(frame, frame, cv::Size(config::RESIZE_WIDTH, config::RESIZE_HEIGHT), (0, 0), (0, 0), cv::INTER_LINEAR);

	std::string frameStrBuf;
	for (int i = 0; i < frame.rows; ++i)
	{
		const uint8_t * pData = frame.ptr<uint8_t>(i);
		for (int k = 0; k < frame.cols; ++k)
		{
			if (pData[k] > config::BIN_THRESHOLD)
				frameStrBuf += config::PRINT_NON_BLANK;
			else
				frameStrBuf += config::PRINT_BLANK;
		}
		frameStrBuf += "\n";
	}

	printf("%s", frameStrBuf.c_str());	//NOTE: 这里使用printf比cout效率高
}

//******************************************************************************************************
//FUNCTION:
bool CConsoleVideoPlayer::play(const std::string& vVideoPath)
{
	_ASSERTE(!vVideoPath.empty());

	char cmd[128];
	sprintf(cmd, "mode con cols=%d lines=%d", config::RESIZE_WIDTH + config::BORDER_SIZE, config::RESIZE_HEIGHT + config::BORDER_SIZE);
	system(cmd);

	cv::VideoCapture videoCapture;
	videoCapture.open(vVideoPath);
	if (!videoCapture.isOpened())
	{
		std::cerr << "failed to open the video!" << std::endl;
		return false;
	}

	long totalFrameNumber = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
	int  frameHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	int  frameWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
	double frameRate = videoCapture.get(CV_CAP_PROP_FPS);

	int startFrame = 0;
	int endFrame = totalFrameNumber;
	videoCapture.set(CV_CAP_PROP_POS_FRAMES, startFrame);

	double deltaTimeInMills = 1000.0 / frameRate;
	for (int i = startFrame; i <= endFrame; ++i)
	{
		auto startTime = std::chrono::high_resolution_clock::now();
		__printVideoFrame(videoCapture);
		auto endTime = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1000>>>(endTime - startTime).count();

		if (duration < deltaTimeInMills) std::this_thread::sleep_for(std::chrono::milliseconds((int)(deltaTimeInMills - duration)));

		std::cout << "FPS: " << 1000.0 / std::max(duration, deltaTimeInMills) << std::endl;
	}

	return true;
}