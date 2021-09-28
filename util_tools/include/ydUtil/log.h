#ifndef __COMMON_LOG_H_FILE__
#define __COMMON_LOG_H_FILE__
#include <ydUtil/export.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <ydUtil/singleton.h>


enum LOG_LEVEL
{
    LL_INFO,
    LL_WARNING,
    LL_ERROR
    
};

class YDUTIL_API SetupLevel
{
public:
	SetupLevel(LOG_LEVEL ll);

	~SetupLevel();
};
#define TRACELL(loglevel, ...) { printf("[%s:%d]\t", __FILE__, __LINE__ ); SetupLevel ll( loglevel ); printf(__VA_ARGS__); }
#define TRACE(...) { TRACELL(LL_INFO, __VA_ARGS__ ); }

namespace ydCommon
{
    class YDUTIL_API TimeTrace
    {
    public:
        TimeTrace();
        ~TimeTrace();
        double getUseTime();
    protected:
#ifdef WIN32
		clock_t _t;
#else
        struct timespec _createTime;
#endif

    };

    YDUTIL_API double currentTime();

    class YDUTIL_API LogToFile : public Singleton < LogToFile >
    {
    protected:
        LogToFile();

        ~LogToFile();
        friend class Singleton<LogToFile>;
    public:
        void setFileName( std::string const& f );
        void log(LOG_LEVEL ll, char* format, ...);

    protected:
        std::string _file;
    };
}

//#define debug(format, args...)       fprintf (stderr, "%s", __FILE__)  

#endif // __COMMON_LOG_H_FILE__
