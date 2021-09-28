#ifndef __YD_UTIL_CONSOLE_H_FILE__
#define __YD_UTIL_CONSOLE_H_FILE__
#include <ydUtil/export.h>
#include <string>
#include <vector>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
YDUTIL_API int _kbhit(void);
#endif
namespace ydUtil
{
#ifdef _WIN32
	typedef struct _dirdesc {
		int     dd_fd;      /** file descriptor associated with directory */
		long    dd_loc;     /** offset in current buffer */
		long    dd_size;    /** amount of data returned by getdirentries */
		char    *dd_buf;    /** data buffer */
		int     dd_len;     /** size of data buffer */
		long    dd_seek;    /** magic cookie returned by getdirentries */
	} DIR;

# define __dirfd(dp)    ((dp)->dd_fd)

	YDUTIL_API DIR *opendir(const char *);
	YDUTIL_API struct dirent *readdir(DIR *);
	YDUTIL_API void rewinddir(DIR *);
	YDUTIL_API int closedir(DIR *);
	YDUTIL_API int createPid( std::string const& appName, std::string const& param = "");
	YDUTIL_API bool closeProcess(int);
	YDUTIL_API int waitpid(int pid, int *status, int flags);
	YDUTIL_API float getProcessMemoryUseage(int pid);


#include <sys/types.h>

	struct dirent
	{
		long d_ino;              /* inode number*/
		off_t d_off;             /* offset to this dirent*/
		unsigned short d_reclen; /* length of this d_name*/
		unsigned char d_type;    /* the type of d_name*/
		char d_name[1];          /* file name (null-terminated)*/
	};
#endif

  YDUTIL_API std::string getBinPath();
  YDUTIL_API void sleep(unsigned int microsecond);
  YDUTIL_API bool fileExist(const char *filePath);
  YDUTIL_API bool dirExist(const char *dirPath);
  YDUTIL_API std::vector<std::string> findDirInPath(const char *dirPath);
  YDUTIL_API bool checkDir(const char *dirPath, bool makeIsMissing = true);
  YDUTIL_API std::string getYearMontDateTimeStr( time_t t = 0 );
  YDUTIL_API std::string getTimeStr();

  // string的编码方式为utf8，则采用：
  YDUTIL_API std::string wstring2utf8string(const std::wstring& str);
  YDUTIL_API std::wstring utf8string2wstring(const std::string& str);
  // string的编码方式为除utf8外的其它编码方式，可采用：
  YDUTIL_API std::string wstring2string(const std::wstring& wstr, const std::string& locale);
  YDUTIL_API std::string wstring2string(const std::wstring& wstr);
  YDUTIL_API void mbtowchar(const char* input, wchar_t* output);
  YDUTIL_API std::wstring string2wstring(const std::string& str, const std::string& locale);
}

#endif // __YD_UTIL_CONSOLE_H_FILE_
