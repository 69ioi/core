#include <ydUtil/console.h>

#ifdef _WIN32
#include <windows.h>
#include <sys/stat.h>
#include <corecrt_io.h>
#include <process.h>
#include <psapi.h>
#include <direct.h>
#include <time.h>
#include <locale>
#include <codecvt>
#else
#include <unistd.h>
#include <sys/stat.h>
#include<fcntl.h>
#include <dirent.h>
#endif

#include <iostream>


#ifndef _WIN32

int _kbhit(void)
{
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);

}

#endif



namespace ydUtil
{
	std::string getBinPath()
	{
		std::string binPath = "";

#ifndef _WIN32
		char processdir[512] = { 0 };
		int len = 511;
		char* path_end;
		if (readlink("/proc/self/exe", processdir, len) <= 0)
			return "";
		path_end = strrchr(processdir, '/');
		if (path_end == NULL)
			return "";
		//++path_end;
		*path_end = '\0';
		binPath = processdir;

#else
		char cp[512] = { 0 };
		GetModuleFileNameA(NULL, cp, 512 - 1);
		(strrchr(cp, '\\')+1)[0] = 0; // 删除文件名，只获得路径字串
		binPath = cp;
#endif
		return binPath;
	}

	void sleep(unsigned int microsecond)
	{
#ifdef _WIN32
		Sleep(microsecond);
#else
		usleep(microsecond * 1000);
#endif

	}

	bool fileExist(const char *filePath)
	{
		if ((access(filePath, 0)) != -1)
		{
			return true;
		}

		return false;
	}
	bool dirExist(const char *dirPath)
	{
		struct stat s;
		stat(dirPath, &s);
		if (s.st_mode & S_IFDIR)
		{
			return true;
		}
		return false;
		//if (_access(dirPath, s.st_mode))
		//{
		//	printf("%s exist\n", dirPath);
		////	return true;
		//}
		//return false;
	}
#ifdef _WIN32
#include <stdio.h>     
#include <windows.h>  


	static HANDLE hFind;

	DIR *opendir(const char *name)
	{
		DIR *dir;
		WIN32_FIND_DATA FindData;
		char namebuf[512];

		sprintf(namebuf, "%s\\*.*", name);

		hFind = FindFirstFile(namebuf, &FindData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			printf("FindFirstFile failed (%d)\n", GetLastError());
			return 0;
		}

		dir = (DIR *)malloc(sizeof(DIR));
		if (!dir)
		{
			printf("DIR memory allocate fail\n");
			return 0;
		}

		memset(dir, 0, sizeof(DIR));
		dir->dd_fd = 0;   // simulate return  

		return dir;
	}

	struct dirent *readdir(DIR *d)
	{
		int i;
		static struct dirent dirent;
		BOOL bf;
		WIN32_FIND_DATA FileData;
		if (!d)
		{
			return 0;
		}

		bf = FindNextFile(hFind, &FileData);
		//fail or end  
		if (!bf)
		{
			return 0;
		}

		for (i = 0; i < 256; i++)
		{
			dirent.d_name[i] = FileData.cFileName[i];
			if (FileData.cFileName[i] == '\0') break;
		}
		dirent.d_reclen = i;
		dirent.d_reclen = FileData.nFileSizeLow;

		//check there is file or directory  
		if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			dirent.d_type = 2;
		}
		else
		{
			dirent.d_type = 1;
		}


		return (&dirent);
	}

	int closedir(DIR *d)
	{
		CloseHandle(hFind);
		if (!d) return -1;

		hFind = 0;
		free(d);
		return 0;
	}

	int createPid(std::string const &appName, std::string const& param)
	{
		char cmdLine[256 * 3];
		sprintf(cmdLine, "%s", (appName + " " + param).c_str());
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;
		si.dwFlags = STARTF_USESHOWWINDOW;//指定wShowWindow成员有效
		si.wShowWindow = TRUE;//此成员设为TRUE的话则显示新建进程的主窗口
		//int pi;
		if (::CreateProcess(
			NULL,//appName.c_str(),		//	指向一个NULL结尾的、用来指定可执行模块的宽字节字符串
			cmdLine,		//	命令行字符串
			NULL,		//	指向一个SECURITY_ATTRIBUTES结构体，这个结构体决定是否返回的句柄可以被子进程继承。
			NULL,		//	如果lpProcessAttributes参数为空（NULL），那么句柄不能被继承。<同上>
			false,		//	指示新进程是否从调用进程处继承了句柄。 
			0,			//	指定附加的、用来控制优先类和进程的创建的标
						//	CREATE_NEW_CONSOLE	新控制台打开子进程
						//	CREATE_SUSPENDED	子进程创建后挂起，直到调用ResumeThread函数
			NULL,		//	指向一个新进程的环境块。如果此参数为空，新进程使用调用进程的环境
			NULL,		//	指定子进程的工作路径
			&si,			//	决定新进程的主窗体如何显示的STARTUPINFO结构体
			&pi			//	接收新进程的识别信息的PROCESS_INFORMATION结构体
		))
		{
		//	printf("start %s failed with error code : %d\n", appName.c_str(), GetLastError());
			return pi.dwProcessId;
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		printf("start %s failed with error code : %d\n", appName.c_str(), GetLastError());
		return 0;
	}

	float getProcessMemoryUseage(int pid)
	{
		uint64_t mem = 0;
		uint64_t vmem = 0;
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
		if (!handle)
		{
			return 0;
		}

		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(handle, &pmc, sizeof(pmc)))
		{
			CloseHandle(handle);
			MEMORYSTATUS ms;
			::GlobalMemoryStatus(&ms);
			mem = pmc.WorkingSetSize;
			float userPercent = float(mem) / ms.dwTotalPhys;
			//vmem = pmc.PagefileUsage;
			return userPercent;
		}
		CloseHandle(handle);
		return 0.0f;
	}

	bool closeProcess(int pid)
	{
		//进程句柄
		//退出代码
		HANDLE handle = OpenProcess(PROCESS_TERMINATE | PROCESS_DUP_HANDLE, NULL, pid);
		if (TerminateProcess(handle, NULL)) {
			CloseHandle(handle);
			return true;

		}
		else {
			CloseHandle(handle);
			return false;
		}

	}


	int waitpid(int pid, int *status, int flags)
	{
		DWORD timeout = INFINITE;
		DWORD waitres;

		(void)status; /* Currently not used by any client here */

		if ((flags | 1) == 1) {
			timeout = 0;
		}
		HANDLE handle = OpenProcess(SYNCHRONIZE | PROCESS_DUP_HANDLE, NULL, pid);
		waitres = WaitForSingleObject(handle, timeout);
		CloseHandle(handle);
		if (waitres == WAIT_OBJECT_0) {
			return pid;
		}
		if (waitres == WAIT_TIMEOUT) {
			return 0;
		}
		return (int)-1;
	}
	
#endif
    std::vector<std::string> findDirInPath(const char *dirPath)
    {
        std::vector<std::string> dirList;
        struct stat dirstat;
        if (stat(dirPath, &dirstat) == -1)
        {
            printf("cant access to %s", dirPath);
            return dirList;
        }

        if (dirstat.st_mode & S_IFDIR)
        {
            struct dirent *entry;
            DIR * dir;
            dir = opendir(dirPath);
            while ((entry = readdir(dir)) != NULL)
            {
                if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))continue;
                struct stat childDirStat;
                std::string childDir = dirPath;
                childDir += std::string("/") + entry->d_name;
                if (stat(childDir.c_str(), &childDirStat) == -1)
                {
                    continue;
                }
                if (childDirStat.st_mode & S_IFDIR)
                {
                    dirList.push_back(entry->d_name);
                }
            }
            closedir(dir);
        }

        return dirList;
    }
    bool checkDir(const char *dirPath, bool makeIsMissing )
    {
        int i,len;
        len = strlen( dirPath );
        char *str = new char[len + 1];
        strncpy(str, dirPath, len );
        str[len] = '\0';

        for( i=0; i < len; i++ )
        {
            if( str[i]=='/' )
            {
                str[i] = '\0';
                if( access(str,0)!=0 )
                {
#ifndef _WIN32
                    mkdir( str, 0777 );
#else
                    _mkdir(str);
#endif
                }
                str[i]='/';
            }
        }
        if( len>0 && access(str,0)!=0 )
		{
#ifndef _WIN32
			mkdir(str, 0777);
#else
			_mkdir(str);
#endif
        }
        delete []str;
        return true;
    }
    std::string getYearMontDateTimeStr( time_t t )
    {
        if ( t == 0 )
        {
            t = time(NULL);
        }
        char ch[64] = {0};
        //char result[100] = {0};
        strftime(ch, sizeof(ch) - 1, "%Y%m%d", localtime(&t));
        //sprintf(result, "%s", ch);
        return std::string(ch);
    }
	
    std::string getTimeStr()
    {
        time_t t = time(NULL);
        char ch[64] = {0};
        char result[100] = {0};
        strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));
        sprintf(result, "%s", ch);
        return std::string(result);
	}

	// string的编码方式为utf8，则采用：
	std::string wstring2utf8string(const std::wstring& str)
	{
		static std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
		return strCnv.to_bytes(str);
	}

	std::wstring utf8string2wstring(const std::string& str)
	{
		static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
		return strCnv.from_bytes(str);
	}

	// string的编码方式为除utf8外的其它编码方式，可采用：
	std::string wstring2string(const std::wstring& wstr, const std::string& locale)
	{
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
		static std::wstring_convert<F> strCnv(new F(locale));

		return strCnv.to_bytes(wstr);
	}

	std::string wstring2string(const std::wstring& wstr)
	{
		std::string result;
		//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
		int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		//宽字节编码转换成多字节编码  
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		//删除缓冲区并返回值  
		result.append(buffer);
		delete[] buffer;
		return result;
	}

	void mbtowchar(const char* input, wchar_t* output) {
		int len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
		MultiByteToWideChar(CP_UTF8, 0, input, -1, output, len);
	}

	std::wstring string2wstring(const std::string& str, const std::string& locale)
	{
		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
		static std::wstring_convert<F> strCnv(new F(locale));

		return strCnv.from_bytes(str);
	}
}
