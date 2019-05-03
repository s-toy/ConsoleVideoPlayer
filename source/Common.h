#pragma once

namespace config
{
	const float CONSOLE_UNIT_ASPECT_RATIO = 7.0f / 3.0f;
	const int VIDEO_RESOLUTION[] = { 480, 360 };
	const float VIDEO_SCALE = 0.1f;
	const int RESIZE_WIDTH = VIDEO_SCALE * VIDEO_RESOLUTION[0] * CONSOLE_UNIT_ASPECT_RATIO;
	const int RESIZE_HEIGHT = VIDEO_SCALE * VIDEO_RESOLUTION[1];
	const int BORDER_SIZE = 10;
	const uint8_t BIN_THRESHOLD = 120;
	const char* PRINT_BLANK = " ";
	const char* PRINT_NON_BLANK = "@";
};