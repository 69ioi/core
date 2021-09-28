#ifndef CURLFILE_H_
#define CURLFILE_H_

#include <ydUtil/export.h>

extern "C" {
#include "curl/curl.h"
}
#include <string>

namespace ydUtil {

	class CurlFile {
	public:
		CurlFile();
		~CurlFile();

    std::string callRemoteAPI();

		double getRemoteFileSize();
		bool download();
		void setNow(long long now);
		long long resumeLocalFileSize();
		bool pause();
		bool resume();
		bool remove();

		//property
		void setUrl(const std::string& url);
		std::string getUrl() const;
		CURLcode urlCode() const;
		long responseCode() const;
		long long Now() const;
		long long totalFileSize() const;
		std::string localPath() const;
		void setLocalPath(const std::string& localPath);
		bool success() const;
		bool cancelDownload() const;
		bool beingDownload() const;

	private:
		CurlFile(const CurlFile&) = delete;
		CurlFile& operator=(const CurlFile&) = delete;

		static size_t receive_data(void *ptr, size_t size, size_t nmemb, void *stream);
		static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
		static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);

		std::string _url;
		std::string _localPath;
		CURLcode _urlCode;
		long _responseCode;
		long long _localFileSize;
		long long _currentDownloadedSize;
		long long _totalFileSize;
		long long _offsetPos;
		bool _cancelDownload;
		bool _beingDownload;
		bool _isSuccess;
		CURL *_curlHandle;

	};
}




#endif
