#include "ydUtil/CurlFile.h"
#include "ydUtil/log.h"
#include <memory>
#include <sys/stat.h>
#include <memory.h>

void FreeFILE(void *ptr)
{
    fclose((FILE *)ptr);
}

typedef void(*FuncPtr)(void *ptr);


namespace ydUtil {
    CurlFile::CurlFile() {
        _url = "";
        _localPath = "" ;
        _urlCode = CURLE_OK;
        _responseCode = 200;
        _localFileSize = 0;
        _currentDownloadedSize = 0;
        _totalFileSize = 0;
        _cancelDownload = false;
        _beingDownload = false;
        _isSuccess = false;
        _curlHandle = NULL;
        curl_global_init(CURL_GLOBAL_ALL);
    }

    CurlFile::~CurlFile() {
        curl_global_cleanup();
    }
    std::string CurlFile::callRemoteAPI( )
    {
        if (_url.empty()) {
            return "";
        }
        CURL *curlHandle = curl_easy_init();
        curl_easy_setopt(curlHandle, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "GET");
        //curl_easy_setopt(curlHandle, CURLOPT_NOBODY, 1);

        curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0);//响应体中是否包含了头信息，比如Content-Type:application/json;charset=UTF-8


        std::string curlReturn;
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, receive_data);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &curlReturn);
        //curl_easy_setopt(curlHandle, CURLOPT_XFERINFOFUNCTION, progress_callback);
        //curl_easy_setopt(curlHandle, CURLOPT_PROGRESSDATA, this);
        //curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 0L);

        CURLcode res = curl_easy_perform(curlHandle);
        if (res != CURLE_OK) {
            TRACE("call %s failed : %d\n", _url.c_str(), res );
        }

        curl_easy_cleanup(curlHandle);

        return curlReturn;
    }

    double CurlFile::getRemoteFileSize() {
        double fileSize = 0.0;
        if (_url.empty()) {
            return fileSize;
        }
        CURL *curlHandle = curl_easy_init();
        curl_easy_setopt(curlHandle, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "GET");
        curl_easy_setopt(curlHandle, CURLOPT_NOBODY, 1);
        if (curl_easy_perform(curlHandle) == CURLE_OK) {
            curl_easy_getinfo(curlHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &fileSize);
        }
        curl_easy_reset(curlHandle);
        curl_easy_cleanup(curlHandle);
        _totalFileSize = static_cast<long long>(fileSize);

        return fileSize;
    }

    bool CurlFile::download() {
        TRACELL(LL_INFO,(_url+"\n").c_str());
        getRemoteFileSize();
        if (_url.empty() || _localPath.empty() || _totalFileSize <= 0) {
            return false;
        }
        long long _offsetPos = 0;
        //FILE* fp = new FILE;
        //fopen_s(&fp, _localPath.c_str(), "ab");
        //std::shared_ptr<FILE> file_obj(fp, [&](void *ptr) { fclose((FILE *)ptr); });
        //std::shared_ptr<FILE> file_obj(fopen(_localPath.c_str(), "ab"), [&](void *ptr) { fclose((FILE *)ptr); });
        std::shared_ptr<FILE> file_obj(fopen(_localPath.c_str(), "ab"), FreeFILE);

        _offsetPos = _localFileSize;
        if (_offsetPos > 0) {
            if (_offsetPos == _totalFileSize) {
                _isSuccess = true;
                return true;
            }
        }
        _curlHandle = curl_easy_init();
        _beingDownload = true;

        CURL *curlHandle = _curlHandle;
        curl_easy_reset(curlHandle);
        /* set URL to get here */
        curl_easy_setopt(curlHandle, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
        /* enable TCP keep-alive for this transfer */
        curl_easy_setopt(curlHandle, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);

        curl_easy_setopt(curlHandle, CURLOPT_FAILONERROR, 1L);
        curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM, _offsetPos);
        curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, _offsetPos);

        curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.96 Safari/537.36");

        /* send all data to this function  */
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        /* write the page body to this file handle */
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, file_obj.get());
        curl_easy_setopt(curlHandle, CURLOPT_XFERINFOFUNCTION, progress_callback);
        curl_easy_setopt(curlHandle, CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 0L);
        CURLcode code = curl_easy_perform(curlHandle);
        if (code == CURLE_OK) {
            code = curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &_responseCode);
        }
        _beingDownload = true;
        curl_easy_cleanup(_curlHandle);
        _curlHandle = NULL;

        _urlCode = code;
        if (code == CURLE_OK) {
            _isSuccess = true;

        }

        return _isSuccess;
    }

    void CurlFile::setNow(long long now) {
        _currentDownloadedSize = now + _localFileSize;
    }

    long long CurlFile::resumeLocalFileSize() {
        long long localSize = 0;
        if (_localPath.empty()) {
            return localSize;
        }

#ifdef _WIN32
        struct _stati64 file_info;
        int state = _stat64(_localPath.c_str(), &file_info);
#else
        struct stat file_info;
        int state = stat(_localPath.c_str(), &file_info);
#endif
        if (0 == state) {
            localSize = file_info.st_size;
            _localFileSize = localSize;
            _currentDownloadedSize = _localFileSize;
        }
        return localSize;
    }

    bool CurlFile::pause() {
        if (!beingDownload()) {
            return false;
        }

        if (NULL == _curlHandle) {
            return false;
        }
        CURLcode code = curl_easy_pause(_curlHandle, CURLPAUSE_RECV);
        if (code != CURLE_OK) {
            return false;
        }
        return true;
    }

    bool CurlFile::resume() {
        if (!beingDownload()) {
            return false;
        }
        if (NULL == _curlHandle) {
            return false;
        }
        CURLcode code = curl_easy_pause(_curlHandle, CURLPAUSE_RECV_CONT);
        if (code != CURLE_OK) {
            return false;
        }
        return true;
    }

    bool CurlFile::remove() {
        if (!beingDownload()) {
            return false;
        }
        _cancelDownload = true;

        return true;
    }

    size_t CurlFile::receive_data(void *ptr, size_t size, size_t nmemb, void *stream )
    {
        std::string *str = (std::string*)stream;
        std::string tmpValue;
        tmpValue.resize( size * nmemb + 1 );
        memcpy( &tmpValue[0], (char*)ptr, size * nmemb );

        (*str) += tmpValue;

        return size * nmemb;
    }

    size_t CurlFile::write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
        size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
        return written;
    }

    int CurlFile::progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        CurlFile *curlFile = static_cast<CurlFile*>(clientp);
        if (NULL != curlFile) {
            if (curlFile->cancelDownload()) {
                return 1;
            }

            if (dlnow > 0) {
                curlFile->setNow(dlnow);
            }
        }
        return 0;
    }

    void CurlFile::setUrl(const std::string& url) {
        TRACELL(LL_INFO,(url + "\n").c_str());
        _url = url;
    }

    std::string CurlFile::getUrl() const {
        return _url;
    }

    CURLcode CurlFile::urlCode() const {
        return _urlCode;
    }

    long CurlFile::responseCode() const {
        return _responseCode;
    }

    long long CurlFile::Now() const {
        return _currentDownloadedSize;
    }

    long long CurlFile::totalFileSize() const {
        return _totalFileSize;
    }

    std::string CurlFile::localPath() const {
        return _localPath;
    }

    void CurlFile::setLocalPath(const std::string & localPath) {
        _localPath = localPath;
    }

    bool CurlFile::success() const {
        return _isSuccess;
    }

    bool CurlFile::cancelDownload() const {
        return _cancelDownload;
    }

    bool CurlFile::beingDownload() const {
        return _beingDownload;
    }


}
