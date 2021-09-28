#include "ydUtil/common.h"

#ifdef _WIN32
#include <regex>
#include <atlstr.h>
#include <direct.h>
#include <corecrt_io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <ydUtil/console.h>
#endif
#include <ydUtil/log.h>

namespace ydUtil
{
	#ifdef _WIN32
	std::string GetCurDir()
	{
		char bp[256] = { 0 };
		_getcwd(bp, 255);
		return bp;
	}

	std::string DeleteFileName(const std::string &pathName)
	{
		char drive[3] = { 0 };
		char dir[256] = { 0 };
		char fname[256] = { 0 };
		char ext[256] = { 0 };
		_splitpath_s(pathName.c_str(), drive, dir, fname, ext);
		return dir;
	}
	#endif

	// 判断参数名对应的目录是否存在，如果不存在则递归创建相应的目录
	bool createMultiDir(const char* pathName)
	{
		/*
		char drive[3] = { 0 };
		char dir[256] = { 0 };
		char fname[256] = { 0 };
		char ext[256] = { 0 };
		_splitpath_s(pathName, drive, dir, fname, ext);

		if (dir == NULL) return false;
		char filePath[256] = { 0 };
		strcat_s(filePath, drive);
		strcat_s(filePath, dir);
		*/
	#ifdef _WIN32
		char filePath[256] = { 0 };
		strcat(filePath, pathName);
		size_t i = 0, pathLen = strlen(filePath);
		CString curPath;
		char curFilePath[256] = { 0 };
		WIN32_FIND_DATA swf;
		if (filePath[pathLen - 1] != '\\')	//最后一个非0字符不是‘\\’则加上
		{
			filePath[pathLen] = '\\';
		}
		while (filePath[i] != '\0')
		{
			if (filePath[i] == ':')
			{
				i += 2;
				continue;
			}
			if (filePath[i] == '\\' || filePath[i] == '/')
			{
				memcpy(curFilePath, filePath, i);
				curFilePath[i] = '\0';
				curPath = curFilePath;
				if (FindFirstFile(curPath, &swf) == INVALID_HANDLE_VALUE) //目录不存在就创建
				{
					if (!CreateDirectory(curPath, NULL))
					{
						return false;
					}
				}
			}
			i++;
		}
	#else
		char   DirName[256];
		strcpy(DirName, pathName);
		int   i, len = strlen(DirName);
		if (DirName[len - 1] != '/')
			strcat(DirName, "/");

		len = strlen(DirName);

		for (i = 1; i < len; i++)
		{
			if (DirName[i] == '/')
			{
				DirName[i] = 0;
				if (access(DirName, NULL) != 0)
				{
					if (mkdir(DirName, 0755) == -1)
					{
						TRACE("Failed to create directory\n");
						return   -1;
					}
				}
				DirName[i] = '/';
			}
		}
	#endif
		return true;
	}

	// 切换工作目录
	std::string switchExpDir(const std::string& folderPath)
	{
		if (!createMultiDir(folderPath.c_str()))
		{
			TRACELL(LL_WARNING, "Directory creation failed\n");
			return "";
		}
	#ifdef _WIN32
		if (_chdir(folderPath.c_str()) != 0)
	#else
		if (chdir(folderPath.c_str()) != 0)
	#endif
		{
			TRACELL(LL_ERROR, "Failed to switch the working directory!\n");
		}
		return folderPath;
	}

	// 解析url，Ex: url="http://192.168.133.129/ab/bc.a"  host="192.168.133.129" uri="ab/bc.a"
	int parseUrl(const std::string &url, std::string &host, std::string &uri)
	{
		char szUrl[256] = { 0 }, szHost[256] = { 0 }, szPath[256] = { 0 };
	#ifdef _WIN32
		strcpy_s(szUrl, url.c_str());
	#else
		strcpy(szUrl, url.c_str());
	#endif

		int iStart = 0;
		int iLen = 0;

		if (strncmp(szUrl, "http://", 7) == 0)
			iStart = 7;
		else if (strncmp(szUrl, "https://", 8) == 0)
			iStart = 8;

		while (szUrl[iStart + iLen] != '\0' && szUrl[iStart + iLen] != '/')
		{
			iLen++;
		}

		memcpy(szHost, szUrl + iStart, iLen);
		if (strlen(szUrl) - iStart - iLen == 0) {
			szPath[0] = '/';
		}
		else {
			// +1 是为了去除host后面的第一个'/'
			memcpy(szPath, szUrl + iStart + iLen + 1, strlen(szUrl) - iStart - iLen);
		}

	#ifdef _WIN32
		std::regex regExpress("(?=(\\b|\\D))(((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))(?=(\\b|\\D))");
		if (std::regex_match(szHost, regExpress))
	#else
		if(strlen(szHost) > 7)
	#endif
		{
			host = szHost;
			uri = szPath;
		}
		else {
			uri = url;
		}
		return 0;
	}

	bool backupFile(const std::string &src, const std::string &dst)
	{
		std::ifstream in;
		in.open(src);
		if (!in)
		{
			TRACELL(LL_WARNING, "Open src file: \"%s\" failed\n", src.c_str());
			return false;
		}
		std::ofstream out;
		out.open(dst);
		if (!out)
		{
			TRACELL(LL_WARNING, "Create backup file: \"%s\" failed\n", dst.c_str());
			in.close();
			return false;
		}
		out << in.rdbuf();
		out.close();
		in.close();
		return true;
	}

	/*
	// 判断文件是否存在
	bool fileExists(const std::string &filePath)
	{
		return _access(filePath.c_str(), 06) == 0;

		std::ifstream in(filePath);
		if (!in)	// 如果文件无法打开，则说明文件不存在
		{
			return false;
		}
		in.close();
		return true;
	}

	// 将字符串中的\\替换为/
	std::string replaceAllword(const std::string& resources, const std::string& key = "\\", const std::string& ReplaceKey = "/")
	{
		size_t pos = 0;
		std::string temp = resources;
		while ((pos = temp.find(key, pos)) != std::string::npos)
		{
			temp.erase(pos, key.size());//删除原有字符串
			temp.insert(pos, ReplaceKey);//插入替换字符串
			pos += ReplaceKey.size(); //更新查询起始标志位
		}
		return temp;
	}
	*/
}
