#include <ydUtil/StringUtils.h>
#include <locale>
namespace ydUtil
{
    std::vector<std::string> split(const std::string& src, const char* seperator, bool ignoreEmpty)
    {
        std::vector<std::string> slist;
        std::string sep = (seperator == NULL) ? " " : std::string(seperator);
        std::string::size_type start = src.find_first_not_of(sep);
        while (start != std::string::npos)
        {
            std::string::size_type end = src.find_first_of(sep, start);
            if (end != std::string::npos)
            {
                slist.push_back(std::string(src, start, end-start));
                if (ignoreEmpty) start = src.find_first_not_of(sep, end);
                else start = end + 1;
            }
            else
            {
                slist.push_back(std::string(src, start, src.size() - start));
                start = end;
            }
        }
        return slist;
    }
    static  const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    YDUTIL_API bool is_base64(unsigned char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    YDUTIL_API std::string base64_encode(unsigned char const *bytes_to_encode, unsigned int in_len)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--)
        {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    YDUTIL_API std::string base64_decode(std::string const &encoded_string)
    {
        size_t in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
        {
            char_array_4[i++] = encoded_string[in_];
            in_++;
            if (i == 4)
            {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
            }
        }

        if (i)
        {
            for (j = 0; j < i; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

            for (j = 0; (j < i - 1); j++)
                ret += char_array_3[j];
        }

        return ret;
    }

    char* itoa(int num,char* str,int radix)
    {
        char index[]="0123456789ABCDEF";
        unsigned unum;
        int i=0,j,k;
       
        if(radix==10&&num<0)
        {
            unum=(unsigned)-num;
            str[i++]='-';
        }
        else unum=(unsigned)num;
        
        do{
            str[i++]=index[unum%(unsigned)radix];
            unum/=radix;
        }while(unum);
        str[i]='\0';
        
        if(str[0]=='-')
            k=1;
        else
            k=0;

        for(j=k;j<=(i-1)/2;j++)
        {       char temp;
            temp=str[j];
            str[j]=str[i-1+k-j];
            str[i-1+k-j]=temp;
        }
        return str;
    }
    std::wstring s2ws(const std::string& str) {
        if (str.empty()) {
            return L"";
        }
        unsigned len = str.size() + 1;
        setlocale(LC_CTYPE, "");
        wchar_t *p = new wchar_t[len];
        mbstowcs(p, str.c_str(), len);
        std::wstring w_str(p);
        delete[] p;
        return w_str;
    }
    std::string ws2s(const std::wstring& w_str) {
        if (w_str.empty()) {
            return "";
        }
        unsigned len = w_str.size() * 4 + 1;
        setlocale(LC_CTYPE, "");
        char *p = new char[len];
        wcstombs(p, w_str.c_str(), len);
        std::string str(p);
        delete[] p;
        return str;
    }
}
