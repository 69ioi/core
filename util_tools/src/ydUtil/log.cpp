#include <ydUtil/log.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <WinSock2.h>
#include <Windows.h>
#endif

#include <fstream>
#include <stdarg.h>
#ifdef _WIN32
SetupLevel::SetupLevel(LOG_LEVEL ll)
{
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);
		
	switch (ll)
	{
	case LL_INFO:
		SetConsoleTextAttribute(hdl, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case LL_WARNING:
		SetConsoleTextAttribute(hdl, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case LL_ERROR:
		SetConsoleTextAttribute(hdl, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	}
	//CloseHandle(hdl);
}

SetupLevel::~SetupLevel()
{
	HANDLE hdl = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hdl, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	//CloseHandle(hdl);
}

#else
SetupLevel::SetupLevel(LOG_LEVEL ll)
{
	switch (ll)
	{
	case LL_INFO:
		printf("\033[32m");
		break;
	case LL_WARNING:
		printf("\033[33m");
		break;
	case LL_ERROR:
		printf("\033[31m");
		break;
	}
}

SetupLevel::~SetupLevel()
{
	printf("\033[0m");
}
#endif
namespace ydCommon
{

	   
    TimeTrace::TimeTrace()
    {
#ifndef WIN32
        clock_gettime( CLOCK_MONOTONIC, &_createTime);
#else
        _t = clock();
#endif

    };
    TimeTrace::~TimeTrace()
    {
#ifndef WIN32
        clock_gettime(CLOCK_MONOTONIC, &_createTime);
#else
 
#endif
    }
    double TimeTrace::getUseTime()
    {
#ifndef WIN32
        long s1 = _createTime.tv_sec;
        double n1 = _createTime.tv_nsec / 1000000000.0;


        struct timespec curTime;
        clock_gettime( CLOCK_MONOTONIC, &curTime );
        long s2 = curTime.tv_sec;
        double cn = curTime.tv_nsec / 1000000000.0;

        _createTime = curTime;

        return cn + s2 - s1 - n1;
#else
        clock_t t = clock();
        _t = t;
        return  t - _t;
#endif

    }
    double currentTime()
    {
#ifndef WIN32
        struct timespec curTime;
        clock_gettime( CLOCK_MONOTONIC, &curTime );
        long s1 = curTime.tv_sec;
        double n1 = curTime.tv_nsec / 1000000000.0;

        return s1 + n1;
#else
        DWORD t = GetTickCount();
        return  t / 1000.0f;
#endif
    }

    LogToFile::LogToFile()
    {
    
    }

    LogToFile::~LogToFile()
    {
    
    }


    void LogToFile::setFileName( std::string const& f )
    {
        _file = f;
    }
    void LogToFile::log(LOG_LEVEL ll, char *format, ...)
    {
        va_list arg_ptr;
        va_start(arg_ptr, format);
        char strInfo[1000] = {0}; // 小心别溢出？
        vsprintf(strInfo, format, arg_ptr); // 按格式打印到strInfo，功能类似sprintf
        va_end(arg_ptr);

        std::ofstream ofs(_file, std::ios::app);
        ofs << "LEVEL:" << ll << " " << strInfo;

    }
}

