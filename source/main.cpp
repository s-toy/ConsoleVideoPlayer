#include <windows.h>
#include <chrono>
#include <ratio>
#include <thread>
#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace std;

namespace config
{
	const float CONSOLE_UNIT_ASPECT_RATIO = 9.0f / 4.0f;
	const int VIDEO_RESOLUTION[] = { 480, 360 };
	const float VIDEO_SCALE = 0.2f;
	const int RESIZE_WIDTH = VIDEO_SCALE * VIDEO_RESOLUTION[0] * CONSOLE_UNIT_ASPECT_RATIO;
	const int RESIZE_HEIGHT = VIDEO_SCALE * VIDEO_RESOLUTION[1];
	const int BORDER_SIZE = 10;
	const uint8_t BIN_THRESHOLD = 120;
	const char* VIDEO_PATH = "../resource/badapple.mp4";
	const char* PRINT_BLANK = " ";
	const char* PRINT_NON_BLANK = "@";
};

static void binaryzation(const cv::Mat& vSrc, cv::Mat& voDst, uint8_t vThreshold);

static void printFrame(cv::VideoCapture& vVideoCapture);

//******************************************************************************************************
//FUNCTION:
int main()
{
	char cmd[128];
	sprintf(cmd, "mode con cols=%d lines=%d", config::RESIZE_WIDTH + config::BORDER_SIZE, config::RESIZE_HEIGHT + config::BORDER_SIZE);
	system(cmd);

	cv::VideoCapture videoCapture;
	videoCapture.open(config::VIDEO_PATH);
	if (!videoCapture.isOpened())
	{
		cerr << "failed to open the video!" << endl;
	}

	long totalFrameNumber = videoCapture.get(CV_CAP_PROP_FRAME_COUNT);
	int  frameHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	int  frameWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
	double frameRate = videoCapture.get(CV_CAP_PROP_FPS);

	int startFrame = 0;
	int endFrame = totalFrameNumber;
	videoCapture.set(CV_CAP_PROP_POS_FRAMES, startFrame);

	double deltaMills = 1000.0 / frameRate;
	for (int i = startFrame; i <= endFrame; ++i)
	{
		auto startTime = chrono::high_resolution_clock::now();
		printFrame(videoCapture);
		auto endTime = chrono::high_resolution_clock::now();

		auto duration = chrono::duration_cast<chrono::duration<double, std::ratio<1, 1000>>>(endTime - startTime).count();

		if (duration < deltaMills) std::this_thread::sleep_for(chrono::milliseconds((int)(deltaMills - duration)));
	}
	return 0;
}

//******************************************************************************************************
//FUNCTION:
void binaryzation(const cv::Mat& vSrc, cv::Mat& voDst, uint8_t vThreshold)
{
	int nr = vSrc.rows;
	int nc = vSrc.cols;
	voDst.create(nr, nc, CV_8UC1);
	for (int i = 0; i < nr; ++i)
	{
		auto dataSrc = vSrc.ptr<uint8_t>(i);
		auto dataDst = voDst.ptr<uint8_t>(i);
		for (int k = 0; k < nc; ++k)
		{
			if (dataSrc[k] >= vThreshold)
				dataDst[k] = 255;
			else
				dataDst[k] = 0;
		}
	}
}

//******************************************************************************************************
//FUNCTION:
void printFrame(cv::VideoCapture& vVideoCapture)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, cursorPos);

	cv::Mat frame;
	vVideoCapture >> frame;

	cv::Mat dst;
	cv::cvtColor(frame, frame, CV_RGB2GRAY);
	cv::resize(frame, dst, cv::Size(config::RESIZE_WIDTH, config::RESIZE_HEIGHT), (0, 0), (0, 0), cv::INTER_LINEAR);
	binaryzation(dst, dst, config::BIN_THRESHOLD);

	int nr = dst.rows;
	int nc = dst.cols;
	uint8_t uMax = 255;
	string outStr;
	for (int i = 0; i < nr; ++i)
	{
		const uint8_t * data = dst.ptr<uint8_t>(i);
		for (int k = 0; k < nc; ++k)
		{
			if (data[k] == uMax)
			{
				outStr += config::PRINT_NON_BLANK;
			}
			else
				outStr += config::PRINT_BLANK;
		}
		outStr += "\n";
	}

	printf("%s", outStr);	//NOTE: 这里使用printf比cout效率高
}