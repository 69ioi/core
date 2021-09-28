#ifndef __COMMON_STRING_UTILS_H_FILE__
#define __COMMON_STRING_UTILS_H_FILE__
#include <ydUtil/export.h>
#include <vector>
#include <string>

namespace ydUtil
{
    YDUTIL_API std::vector<std::string> split(const std::string& src, const char* seperator, bool ignoreEmpty = true);
    //////////////// BASE64 support
    YDUTIL_API bool is_base64(unsigned char c);
    YDUTIL_API std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len);
    YDUTIL_API std::string base64_decode(std::string const &encoded_string);

    YDUTIL_API char* itoa(int num,char* str,int radix);

    YDUTIL_API std::wstring s2ws(const std::string& str);
    YDUTIL_API std::string ws2s(const std::wstring& w_str);
}

#endif // __COMMON_STRING_UTILS_H_FILE__
