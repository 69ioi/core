#ifndef __YDUTIL_DATA_READ_WRITE_H_FILE__
#define __YDUTIL_DATA_READ_WRITE_H_FILE__
#include <ydUtil/export.h>
#include <iostream>


namespace ydUtil
{
    class YDUTIL_API DataReadWriter
    {
    public:

    public:
        void read( std::istream &s, std::string &v );
        void write(std::ostream &s, std::string const &v);

        void read(std::istream &s, unsigned char* buff, unsigned int len);
        void write(std::ostream &s, unsigned char* buff, unsigned int len);

        template <class T>
            void read(std::istream &s, T &v, int len)
        {
            s.read((char*)&v, len );
        }
        template <class T>
            void write(std::ostream &s, T const &v, int len)
        {
            s.write((const char*)&v, len );
        }

        template <class T>
            void read(std::istream &s, T &v)
        {
            s.read((char*)&v, sizeof( T ) );
        }
        template <class T>
            void write(std::ostream &s, T const &v)
        {
            s.write((const char*)&v, sizeof ( T ) );
        }

        void streamOut(std::ostream &s, char const *v);

    };


#endif // __YDUTIL_DATA_READ_WRITE_H_FILE__
}
