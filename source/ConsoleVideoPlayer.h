#pragma once
#include <string>

namespace cvplayer
{
	class CConsoleVideoPlayer
	{
	public:
		static CConsoleVideoPlayer& getInstance()
		{
			static CConsoleVideoPlayer instance;
			return instance;
		}

		bool play(const std::string& vVideoPath);
	};
}