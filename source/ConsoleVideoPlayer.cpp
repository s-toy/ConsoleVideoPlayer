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
static void __binaryzation(const cv::Mat& vSrc, cv::Mat& voDst, uint8_t vThreshold)
{
	int nr = vSrc.rows;
	int nc = vSrc.cols;
	voDst.create(nr, nc, CV_8UC1);

	for (int i = 0; i < nr; ++i)
	{
		auto pDataSrc = vSrc.ptr<uint8_t>(i);
		auto pDataDst = voDst.ptr<uint8_t>(i);

		for (int k = 0; k < nc; ++k) pDataDst[k] = pDataSrc[k] >= vThreshold ? 255 : 0;
	}
}

//******************************************************************************************************
//FUNCTION:
static void __printVideoFrame(cv::VideoCapture& vVideoCapture)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, cursorPos);

	cv::Mat frame;
	vVideoCapture >> frame;

	cv::Mat dst;
	cv::cvtColor(frame, frame, CV_RGB2GRAY);
	cv::resize(frame, dst, cv::Size(config::RESIZE_WIDTH, config::RESIZE_HEIGHT), (0, 0), (0, 0), cv::INTER_LINEAR);
	__binaryzation(dst, dst, config::BIN_THRESHOLD);

	int nr = dst.rows;
	int nc = dst.cols;
	uint8_t uMax = 255;
	std::string outStr;
	for (int i = 0; i < nr; ++i)
	{
		const uint8_t * pData = dst.ptr<uint8_t>(i);
		for (int k = 0; k < nc; ++k)
		{
			if (pData[k] == uMax)
				outStr += config::PRINT_NON_BLANK;
			else
				outStr += config::PRINT_BLANK;
		}
		outStr += "\n";
	}

	printf("%s", outStr);	//NOTE: 这里使用printf比cout效率高
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