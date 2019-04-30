#include "ConsoleVideoPlayerInterface.h"
#include "ConsoleVideoPlayer.h"

//******************************************************************************************************
//FUNCTION:
bool cvplayer::playVideo(const std::string& vVideoPath)
{
	return cvplayer::CConsoleVideoPlayer::getInstance().play(vVideoPath);
}