#include "ydUtil/DataReadWriter.h"
#include <iostream>

namespace ydUtil
{
    void DataReadWriter::write(std::ostream &s, std::string const &v)
    {
        std::ostream &ss = (s);
        unsigned int len = v.length();
        write(s, len);
        if ( len > 0 )
            ss.write(v.c_str(), len);
    }
    void DataReadWriter::read(std::istream &s, std::string &v)
    {
        std::istream &ss = (s);
        unsigned int len = 0;
        read(s, len);

        if (len > 0)
        {
            v.resize(len);
            ss.read(&v.at(0), len);
        }

    }
    void DataReadWriter::read(std::istream &s, unsigned char* buff, unsigned int len)
    {
        s.read((char*)buff, len);
    }
    void DataReadWriter::write(std::ostream &s, unsigned char* buff, unsigned int len)
    {
        s.write((char*)buff, len);
    }
    void DataReadWriter::streamOut(std::ostream &s, char const *v)
    {
        s << v;
    }
}
