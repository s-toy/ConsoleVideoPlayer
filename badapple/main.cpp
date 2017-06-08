#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>

using namespace cv;
using namespace std;

VideoCapture capture;
const int resize_height = 0.4 * 108;
const int resize_width = 0.4 * 144 * 5 / 3.0;
const uint8_t bin_threshold = 120;
//二值化处理
void binaryzation(const cv::Mat& src, cv::Mat& dst, uint8_t threshold);

//打印当前帧
void printFrame();

int main() {
	char cmd[128];
	sprintf(cmd, "mode con cols=%d lines=%d", resize_width + 10, resize_height + 10);
	system(cmd);

	capture.open("badapple.mp4");
	if (!capture.isOpened())
		cout << "视频打开失败!" << endl;

	//获取整个帧数
	long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);

	// 获取视频相关信息-帧像素宽高   
	int  frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	int  frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);

	// 获取视频相关信息-帧率  
	double frameRate = capture.get(CV_CAP_PROP_FPS);

	int startFrame = 24*10;
	int endFrame = totalFrameNumber;
	float fps = frameRate;
	capture.set(CV_CAP_PROP_POS_FRAMES, startFrame);

	for (int i = startFrame; i <= endFrame; ++i) {
		printFrame();
		//getchar();
		//waitKey(1000 / fps);
	}
	return 0;
}

void binaryzation(const cv::Mat& src, cv::Mat& dst, uint8_t threshold) {
	int nr = src.rows;
	int nc = src.cols;
	dst.create(nr, nc, CV_8UC1);
	for (int i = 0; i < nr; i++) {
		const uint8_t * dataSrc = src.ptr<uint8_t>(i);
		uint8_t * dataDst = dst.ptr<uint8_t>(i);
		for (int j = 0; j < nc; j++) {
			if (dataSrc[j] >= threshold) {
				dataDst[j] = 255;
			}
			else {
				dataDst[j] = 0;
			}
		}
	}
}

void printFrame() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursorPos = { 0, 0 };
	SetConsoleCursorPosition(hConsole, cursorPos);

	Mat frame;
	capture >> frame;

	Mat dst;
	//imshow("src", frame);
	cvtColor(frame, frame, CV_RGB2GRAY);
	//cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0.0);
	cv::resize(frame, dst, cv::Size(resize_width, resize_height), (0, 0), (0, 0), cv::INTER_LINEAR);
	binaryzation(dst, dst, bin_threshold);
	//imshow("dst", dst);

	int nr = dst.rows;
	int nc = dst.cols;
	uint8_t uMax = 255;
	string outStr = "";
	for (int i = 0; i < nr; i++) {
		const uint8_t * data = dst.ptr<uint8_t>(i);
		for (int j = 0; j < nc; j++) {
			if (data[j] == uMax) {
				outStr += "@";
			}
			else
				outStr += " ";
		}
		outStr += "\n";
	}

	printf("%s", outStr);
}