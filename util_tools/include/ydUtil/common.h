#ifndef __EXPANSION_COMMON_H__
#define __EXPANSION_COMMON_H__
#include <ydUtil/export.h>
#include <map>
#include <vector>
#include <fstream>
#include <osg/Vec3f>
#include <osg/Vec3d>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#endif
namespace ydUtil
{
	// 文件描述
	typedef struct FILEINFO
	{
		std::string fileSize;
		std::string filePath;
		std::string accessTime;

		bool operator==(struct FILEINFO& fileInfo) {
			if (this->accessTime == fileInfo.accessTime && this->filePath == fileInfo.filePath && this->fileSize == fileInfo.fileSize)
				return true;
			else
				return false;
		}
		bool operator!=(struct FILEINFO& fileInfo) {
			if (!(*this == fileInfo))
				return true;
			else
				return false;
		}
		bool empty() {
			if (this->accessTime.empty() && this->filePath.empty() && this->fileSize.empty())
				return true;
			else
				return false;
		}

	}FileInfo;
	typedef std::map<std::string, FileInfo> FileInfos;

	// 目录/文件版本
	typedef struct VERSION {
		std::string version;
		FileInfos fileInfos;
		bool empty() {
			if (this->version.empty() && this->fileInfos.empty())
				return true;
			else
				return false;
		}
	}Files;

#ifdef _WIN32
	static std::string GetCurDir();

	static std::string DeleteFileName(const std::string &pathName);
#endif

	YDUTIL_API bool createMultiDir(const char* pathName);
	YDUTIL_API std::string switchExpDir(const std::string& folderPath);
	YDUTIL_API int parseUrl(const std::string &url, std::string &host, std::string &path);
	YDUTIL_API bool backupFile(const std::string &src, const std::string &dst);

	/*
	static bool fileExists(const std::string &filePath);
	static std::string replaceAllword(const std::string& resources, const std::string& key = "\\", const std::string& ReplaceKey = "/");
	*/
}

#endif // __EXPANSION_COMMON_H__
