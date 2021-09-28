#ifndef DOWNLOAD_TASK_H_
#define DOWNLOAD_TASK_H_
#include <ydUtil/export.h>
#include <thread>
#include <vector>
#include <string>

namespace ydUtil {
	class CurlFile;
	class YDUTIL_API DownloadTask {
	public:
		DownloadTask(const std::string &host = "http://127.0.0.1/");
		~DownloadTask();

		std::string downloadFile(std::string url);
		void downloadFile(std::string host, std::string path);
		std::string getCurDir() {return _curDir;}

    std::string callRemoteAPI( std::string url );

	private:
		bool addSeed(const std::string& url, const std::string& localPath);
		double getProgressValue();
		void beginTask();
		void pauseTask();
		void resumeTask();
		void removeTask();
		long long getTaskDownloadSize();
		long long getTaskTotalSize();

		DownloadTask(const DownloadTask&) = delete;
		DownloadTask& operator=(const DownloadTask&) = delete;

		void run();
		void progressBar();

		std::string _host;
		std::string _curDir;

		std::vector<std::unique_ptr<CurlFile> > _seeds;
		bool _isRuning;
		std::thread _taskThread;
	};
}

#endif // DOWNLOAD_TASK_H_
