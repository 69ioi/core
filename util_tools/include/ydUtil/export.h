#ifndef __YDUTIL_EXPORT_H_FILE__
#define __YDUTIL_EXPORT_H_FILE__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef YDUTIL_LIBRARY
#    define YDUTIL_API __declspec(dllexport)
#  else
#    define YDUTIL_API __declspec(dllimport)
#  endif
#else
#  ifdef YDUTIL_LIBRARY
#    define YDUTIL_API  __attribute__ ((visibility("default")))
#  else
#    define YDUTIL_API
#  endif
#endif


#endif //__YDUTIL_EXPORT_H_FILE__
