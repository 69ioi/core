#include "ydUtil/DownloadTask.h"
#include "ydUtil/CurlFile.h"
#include "ydUtil/console.h"
#include "ydUtil/common.h"
#include <ydUtil/log.h>
#include <osgDB/FileNameUtils>
#include <functional>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#endif // _WIN32

namespace ydUtil {
    DownloadTask::DownloadTask(const std::string &host) {
        _host = host;
        _curDir = getBinPath();
        switchExpDir(_curDir);
        _curDir += "/cache/";
    }

    DownloadTask::~DownloadTask() {
        if (_taskThread.joinable()) {
            _taskThread.join();
        }
    }


    // http://127.0.0.1:3306/a/b.c -> 127.0.0.1_3306/a/b.c
    std::string DownloadTask::downloadFile(std::string url)
    {
        std::string host, uri;
        parseUrl(url, host, uri);
        uri = _curDir + host  + "/" + uri;
        if ( uri.find(":") != std::string::npos )
        {
            uri.replace(uri.find(":"), 1, "_");
        }
        std::string path, fileName;
        path = osgDB::getFilePath(uri);
        fileName = osgDB::getSimpleFileName(uri);

#ifdef _WIN32
        if (_access(uri.c_str(), 06) != 0)		// 判断文件是否存在，文件不存在，则加入到下载队列
#else
            if (access(uri.c_str(), F_OK) != 0)
#endif
            {
                if(!createMultiDir(path.c_str()))
                    return "";
                std::shared_ptr<CurlFile> curl(new CurlFile);
                curl->setUrl(url);
                curl->setLocalPath(uri);
                if(!curl->download())
                    TRACE("%s：下载失败\n", fileName.c_str());
            }
        return uri;
    }

    void DownloadTask::downloadFile(std::string host, std::string path)
    {
        std::string url;
        if (host == "") {
            host = _host;
        }
        if (path[0] == '.' && path[1] == '/')
        {
            path.erase(0, 2);	// 擦除字符 "./"，方便拼接url
        }
        url = _host + path;

        if (osgDB::getFileExtensionIncludingDot(path) == ".ept")	// 如果path路径为.ept后缀，则不用下载
        {
            return;
        }
        if (osgDB::getFileExtensionIncludingDot(path) == ".eptile")	// 判断是否包含.eptile后缀，包含则去掉后缀
        {
            path.erase(path.find(".eptile", 0));
            url.erase(url.find(".eptile", 0));
#ifdef _WIN32
            if (_access(path.c_str(), 06) != 0)	return;
#else
            if (access(path.c_str(), F_OK) != 0) return;
#endif
        }
#ifdef _WIN32
        if (_access(path.c_str(), 06) != 0)		// 判断文件是否存在，文件不存在，则加入到下载队列
#else
            if (access(path.c_str(), F_OK) != 0)
#endif
            {
                createMultiDir(path.c_str());
                std::shared_ptr<CurlFile> curl(new CurlFile);
                curl->setUrl(url);
                curl->setLocalPath(path);
                curl->download();
                //addSeed(url, path);
                //beginTask();
                progressBar();
            }
    }

    bool DownloadTask::addSeed(const std::string & url, const std::string & local_path) {
        if (_isRuning) {
            // return false;
        }
        std::unique_ptr<CurlFile> curl(new CurlFile());
        curl->setUrl(url);
        curl->setLocalPath(local_path);
        _seeds.emplace_back(std::move(curl));

        return true;
    }

    double DownloadTask::getProgressValue() {
        long long total = 0;
        long long now = 0;
        long long temp_total_size = 0;
        double progress = 0.0;
        if (_seeds.size() == 0)
        {
            return 101.0;
        }
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            temp_total_size = (*iter)->totalFileSize();
            if (temp_total_size <= 0) {
                return progress;
            }
            total += temp_total_size;
            now += (*iter)->Now();
        }

        if (total > 0) {
            progress = (now / static_cast<double long>(total)) * 100;
        }

        return progress;
    }

    void DownloadTask::beginTask() {
        if (_isRuning) {
            return;
        }
        std::thread task_thread(std::bind(&DownloadTask::run, this));
        _isRuning = true;
        _taskThread = std::move(task_thread);
    }

    void DownloadTask::pauseTask() {
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            (*iter)->pause();
        }
    }

    void DownloadTask::resumeTask() {
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            (*iter)->resume();
        }
    }

    void DownloadTask::removeTask() {
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            (*iter)->remove();
        }
    }

    long long DownloadTask::getTaskDownloadSize() {
        long long now = 0;
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            now += (*iter)->Now();
        }
        return now;
    }

    long long DownloadTask::getTaskTotalSize() {
        long long total = 0;
        long long temp_total_size = 0;
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            temp_total_size = (*iter)->totalFileSize();
            total += temp_total_size;
        }
        return total;
    }

    void DownloadTask::run() {
        TRACELL(LL_INFO,"Begin Download\n");
        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            (*iter)->getRemoteFileSize();
            (*iter)->resumeLocalFileSize();
        }

        for (auto iter = _seeds.begin(); iter != _seeds.end(); ++iter) {
            if (!(*iter)->success()) {
                bool is_fair = (*iter)->download();//下载是阻塞的
                if (!is_fair) {
                    break;
                }
            }
        }

    }

    void DownloadTask::progressBar()
    {
        time_t first;
        first = time(NULL);
        while (true)
        {
            long long preSize = 0;
            double curProgressValue = getProgressValue();
            if (curProgressValue > 1 && curProgressValue <= 100.0)
            {
                TRACELL(LL_WARNING, "正在下载,下载进度:%6.2lf%%,下载速度:%9.2lfKB/s\r",
                        curProgressValue,
                        (getTaskDownloadSize() - preSize) / (double)((time(NULL) - first) * 1024));
                preSize = getTaskDownloadSize();
                first = time(NULL);
                sleep(50);
            }
            if (curProgressValue >= 100.0)
            {
                TRACE("下载完成！\n")
                    printf("\n");
                break;
            }
        }
    }

    std::string DownloadTask::callRemoteAPI( std::string url )
    {
        CurlFile cf;
        cf.setUrl( url );
        return cf.callRemoteAPI();
    }

}
