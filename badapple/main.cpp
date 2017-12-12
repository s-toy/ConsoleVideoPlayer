#include <windows.h>
#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace std;

namespace
{
	const float CONSOLE_UNIT_ASPECT_RATIO = 7.0f / 3.0f;
	const int VIDEO_RESOLUTION[] = { 512, 384 };
	const float VIDEO_SCALE = 0.42f;
	const int RESIZE_WIDTH = VIDEO_SCALE * VIDEO_RESOLUTION[0] * CONSOLE_UNIT_ASPECT_RATIO;
	const int RESIZE_HEIGHT = VIDEO_SCALE * VIDEO_RESOLUTION[1];
	const int BORDER_SIZE = 10;
	const uint8_t BIN_THRESHOLD = 120;
	const char* VIDEO_PATH = "badapple.mp4";
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
	sprintf(cmd, "mode con cols=%d lines=%d", RESIZE_WIDTH + BORDER_SIZE, RESIZE_HEIGHT + BORDER_SIZE);
	system(cmd);

	cv::VideoCapture videoCapture;
	videoCapture.open(VIDEO_PATH);
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
	float fps = frameRate;
	videoCapture.set(CV_CAP_PROP_POS_FRAMES, startFrame);

	float deltaMills = 1000.0f / fps;
	for (int i = startFrame; i <= endFrame; ++i)
	{
		clock_t startTime = clock();
		printFrame(videoCapture);
		clock_t endTime = clock();

		auto printMills = 1000.0f * (float)(endTime - startTime) / CLOCKS_PER_SEC;
		cout << printMills << endl;
		if (printMills < deltaMills)
			Sleep(deltaMills - printMills);
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
	cv::resize(frame, dst, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT), (0, 0), (0, 0), cv::INTER_LINEAR);
	binaryzation(dst, dst, BIN_THRESHOLD);

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
				outStr += PRINT_NON_BLANK;
			}
			else
				outStr += PRINT_BLANK;
		}
		outStr += "\n";
	}

	printf("%s", outStr);	//这里使用printf比cout效率高
}