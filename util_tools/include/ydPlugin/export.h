#ifndef __YD_PLUGIN_EXPORT_H_FILE__
#define __YD_PLUGIN_EXPORT_H_FILE__

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef YDPLUGIN_LIBRARY
#    define YDPLUGIN_API __declspec(dllexport)
#  else
#    define YDPLUGIN_API __declspec(dllimport)
#  endif
#else
#  ifdef YDPLUGIN_LIBRARY 
#    define YDPLUGIN_API  __attribute__ ((visibility("default")))
#  else
#    define YDPLUGIN_API
#  endif
#endif


#endif //__YD_PLUGIN_EXPORT_H_FILE__
