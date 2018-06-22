/**
@brief Common.hpp
Commonly used class and def
@author zhu-ty
@date Jun 22, 2018
*/

#ifndef __FILE_SAVE_SIMULATOR_COMMON__
#define __FILE_SAVE_SIMULATOR_COMMON__

#pragma once
// include stl
#include <memory>
#include <vector>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <cmath>
#include <string>
#include <time.h>
#include <algorithm> //transform
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef WIN32
#define BLACK_TEXT(x) "\033[30;1m" << x << "\033[0m"
#define RED_TEXT(x) "\033[31;1m" << x << "\033[0m"
#define GREEN_TEXT(x) "\033[32;1m" <<  x << "\033[0m"
#define YELLOW_TEXT(x) "\033[33;1m" << x << "\033[0m"
#define BLUE_TEXT(x) "\033[34;1m" << x << "\033[0m"
#define MAGENTA_TEXT(x) "\033[35;1m" << x << "\033[0m"
#define CYAN_TEXT(x) "\033[36;1m" << x << "\033[0m"
#define WHITE_TEXT(x) "\033[37;1m" << x << "\033[0m"
#endif


class SysUtil {
private:
	enum class ConsoleColor {
		red = 12,
		blue = 9,
		green = 10,
		yellow = 14,
		white = 15,
		pink = 13,
		cyan = 11
	};
public:
	/***********************************************************/
	/*                    mkdir function                       */
	/***********************************************************/
	static int mkdir(char* dir) {
#ifdef WIN32
		_mkdir(dir);
#else
		char command[256];
		sprintf(command, "mkdir %s", dir);
		system(command);
#endif
		return 0;
	}
	static int mkdir(std::string dir) {
		return mkdir((char *)dir.c_str());
	}

	/***********************************************************/
	/*                    sleep function                       */
	/***********************************************************/
	static int sleep(size_t miliseconds) {
		std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
		return 0;
	}

	/***********************************************************/
	/*             make colorful console output                */
	/***********************************************************/
	static int setConsoleColor(ConsoleColor color) {
#ifdef WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<int>(color));
#endif
		return 0;
	}

	/***********************************************************/
	/*                 warning error output                    */
	/***********************************************************/
	static int errorOutput(std::string info) {
#ifdef WIN32
		SysUtil::setConsoleColor(ConsoleColor::red);
		std::cerr << "ERROR: " << info.c_str() << std::endl;
		SysUtil::setConsoleColor(ConsoleColor::white);
#else
		std::cerr << RED_TEXT("ERROR: ") << RED_TEXT(info.c_str())
			<< std::endl;
#endif
		return 0;
	}

	static int warningOutput(std::string info) {
#ifdef WIN32
		SysUtil::setConsoleColor(ConsoleColor::yellow);
		std::cerr << "WARNING: " << info.c_str() << std::endl;
		SysUtil::setConsoleColor(ConsoleColor::white);
#else
		std::cerr << YELLOW_TEXT("WARNING: ") << YELLOW_TEXT(info.c_str())
			<< std::endl;
#endif
		return 0;
	}

	static int infoOutput(std::string info) {
#ifdef WIN32
		SysUtil::setConsoleColor(ConsoleColor::green);
		std::cerr << "INFO: " << info.c_str() << std::endl;
		SysUtil::setConsoleColor(ConsoleColor::white);
#else
		std::cerr << GREEN_TEXT("INFO: ") << GREEN_TEXT(info.c_str())
			<< std::endl;
#endif
		return 0;
	}

	static int debugOutput(std::string info) {
#ifdef WIN32
		SysUtil::setConsoleColor(ConsoleColor::pink);
		std::cerr << "DEBUG INFO: " << info.c_str() << std::endl;
		SysUtil::setConsoleColor(ConsoleColor::white);
#else
		std::cerr << MAGENTA_TEXT("DEBUG INFO: ") << MAGENTA_TEXT(info.c_str())
			<< std::endl;
#endif
		return 0;
	}

	static std::string getTimeString()
	{
		time_t timep;
		time(&timep);
		char tmp[64];
		strftime(tmp, sizeof(tmp), "__%Y_%m_%d_%H_%M_%S__", localtime(&timep));
		return tmp;
	}
};

#endif // !__FILE_SAVE_SIMULATOR_COMMON__