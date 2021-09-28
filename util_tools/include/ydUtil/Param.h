#ifndef __YDUTIL_PARAM_H_FILE__
#define __YDUTIL_PARAM_H_FILE__
#include <ydUtil/export.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <vector>
#include <map>
#include <string>
#include <functional>

namespace ydUtil
{
    class YDUTIL_API Param : public osg::Referenced
    {

    public:
        enum ParamType
        {
            PT_NONE,
            PT_LEAF,
            PT_VECTOR,
            PT_MAP,
        };

        Param( ParamType pt );
        virtual ~Param();

        unsigned int getType();

        virtual std::string value() = 0;
        virtual std::string toString() = 0;
        static Param* fromString( std::string const& );
    protected:
        unsigned int _type;

    };
    class YDUTIL_API LeafParam : public Param
    {
    public:
        LeafParam( std::string const& value = "" );
        void set( std::string const& value );
        std::string get()const;

        virtual std::string value(){ return get(); };
        virtual std::string toString();
    protected:
        std::string _value;

    };
    class YDUTIL_API VectorParam : public Param
    {
    public:
        VectorParam();

        unsigned int getCount();

        Param* operator[]( unsigned int index );
        void add( std::string const& value );
        void add( Param *lpParam );
        Param* get(unsigned int index);

        virtual std::string value(){ return "VectorParam"; };
        virtual std::string toString();

        using eachfunc = std::function<void( Param* )>;
        void for_each( eachfunc func );
    protected:
        std::vector<osg::ref_ptr<Param>> _params;

    };
    class YDUTIL_API MapParam : public Param
    {
    public:
        MapParam();
        Param *operator[]( std::string const& key );
        void set( std::string const& group, std::string const& key, std::string const& value );

        void set( std::string const& key, std::string const& value );
        void set( std::string const& key, Param *lpParam );

        Param *get( std::string const& key );
        std::string get( std::string const& group, std::string const& key );
        std::string getValue( std::string const& key );

        virtual std::string value(){ return "MapParam"; };
        virtual std::string toString();

        using eachfunc = std::function<void( std::string const&, Param* )>;
        void for_each( eachfunc func );
    protected:
        std::map<std::string, osg::ref_ptr<Param> > _params;

    };
}


#endif // __YDUTIL_PARAM_H_FILE__
