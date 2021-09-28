#include <ydUtil/Param.h>
#include <picojson.h>
#include <ydUtil/StringUtils.h>

namespace ydUtil
{
    Param *fromPicoJson( picojson::value const& value )
    {
        if ( value.is<picojson::object>() )
        {
            picojson::object jsonMap = value.get<picojson::object>();
            MapParam *lpMapParam = new MapParam;
            for (picojson::value::object::const_iterator i = jsonMap.begin(); i != jsonMap.end(); ++i)
            {
                lpMapParam->set( i->first, fromPicoJson( i->second ) );
            }
            return lpMapParam;

        }else if ( value.is<picojson::array>() )
        {
            picojson::array jsonArray = value.get<picojson::array>();
            VectorParam *lpVP = new VectorParam;
            for( picojson::array::const_iterator it = jsonArray.begin(); it != jsonArray.end(); it ++ )
            {
                lpVP->add( fromPicoJson( (*it) ) );
            }
            return lpVP;
        }

        LeafParam *leafParam = new LeafParam;
        leafParam->set(value.to_str());
        return leafParam;

    }

    Param *Param::fromString( std::string const& s )
    {
        picojson::value hierarchyJson;
        std::string stat2 = picojson::parse(hierarchyJson, s);
        if ( !stat2.empty() )
        {
            std::cout << "Failed to parse param : " << s << std::endl;
            return NULL;
        }

        return fromPicoJson( hierarchyJson );
    }

    Param::Param( ParamType pt )
        : _type( pt )
    {

    }
    Param::~Param()
    {

    }

    unsigned int Param::getType()
    {
        return _type;
    }

////////////////////////////////////////////////////////////////
    LeafParam::LeafParam( std::string const& value )
        : Param( PT_LEAF )
        , _value( value )
    {

    }
    void LeafParam::set( std::string const& value )
    {
        _value = value;
    }

    std::string LeafParam::get()const
    {
        return _value;
    }

    std::string LeafParam::toString()
    {
        return "\"" + _value + "\"";
    }
//////////////////////////////////////////////////////////////////
    VectorParam::VectorParam()
        : Param( PT_VECTOR )
    {

    }

    unsigned int VectorParam::getCount()
    {
        return _params.size();
    }

    Param* VectorParam::operator[]( unsigned int index )
    {
        return get( index );
    }

    void VectorParam::add( std::string const& value )
    {
        _params.push_back( new LeafParam( value ) );
    }
    void VectorParam::add( Param *lpParam )
    {
        if ( !lpParam )
        {
            return;
        }
        _params.push_back( lpParam );
    }
    Param* VectorParam::get(unsigned int index)
    {
        if ( index >= _params.size() )
        {
            return NULL;
        }

        return _params[index].get();
    }
    std::string VectorParam::toString()
    {
        std::string result = "[";
        std::string c = std::to_string( _params.size() );

        for ( unsigned int i = 0; i < _params.size(); i ++ )
        {
            if ( i == _params.size() - 1 )
            {
                result += _params[i]->toString();
            }else
            {
                result += _params[i]->toString() + ",";
            }
        }
        result += "]";

        return result;
    }

    void VectorParam::for_each( eachfunc func )
    {
        if ( !func )
        {
            return;
        }
        for ( auto &p : _params )
        {
            func ( p );
        }
    }
/////////////////////////////////////////////////////////////////

    MapParam::MapParam()
        : Param( PT_MAP )
    {

    }
    Param *MapParam::operator[]( std::string const& key )
    {
        return get( key );
    }
    void MapParam::set( std::string const& group, std::string const& key, std::string const& value )
    {
        if ( _params.find( group ) == _params.end() )
        {
            MapParam *lpMapParam = new MapParam;
            _params[group] = lpMapParam;
        }
        MapParam *lpMapParam = dynamic_cast<MapParam*>( get(group) );
        if ( !lpMapParam )
        {
            lpMapParam = new MapParam;
            _params[group] = lpMapParam;
        }

        lpMapParam->set( key, value );
    }
    void MapParam::set( std::string const& key, std::string const& value )
    {
        LeafParam *lp = new LeafParam;
        lp->set( value );
        _params[key] = lp;
    }
    void MapParam::set( std::string const& key, Param *lpParam )
    {
        if ( !lpParam )
        {
            return;
        }
        _params[key] = lpParam;
    }

    Param *MapParam::get( std::string const& key )
    {
        std::vector<std::string> keyList = ydUtil::split( key, "/" );
        for ( std::vector<std::string>::iterator it = keyList.begin(); it != keyList.end(); )
        {
            if ( (*it) == "" )
            {
                it = keyList.erase( it );
            }else
            {
                it ++;
            }
        }

        MapParam *lpParam = this;
        for ( auto k : keyList )
        {
            if ( lpParam->_params.find( k ) == lpParam->_params.end() )
            {
                return NULL;
            }

            if ( k != keyList[keyList.size() - 1 ] )
            {
                lpParam = dynamic_cast<MapParam*>((lpParam->_params)[k].get());
                if ( !lpParam )
                {
                    return NULL;
                }
            }else
            {
                Param *lpResultParam = lpParam->_params[k];
                return lpResultParam;
            }
        }

        return NULL;

        if ( _params.find( key ) == _params.end() )
        {
            return NULL;
        }
        return _params[key].get();
    }

    std::string MapParam::getValue( std::string const& key )
    {
        Param *lpParam = get( key );
        if ( !lpParam )
        {
            return "";
        }

        return lpParam->value();
    }
    std::string MapParam::get( std::string const& group, std::string const& key )
    {
        MapParam *lpMapParam = dynamic_cast<MapParam*>(get(group));
        if ( !lpMapParam )
        {
            return "";
        }

        Param *lpParam = lpMapParam->get(key);
        if ( !lpParam )
        {
            return "";
        }
        return lpParam->value();
    }

    std::string MapParam::toString()
    {
        std::string str = "{";

        std::vector< std::string > sv;
        for ( auto &i : _params )
        {
            sv.push_back (  "\"" + i.first + "\":" + i.second->toString() );
        }
        for ( unsigned int i = 0; i < sv.size(); i ++ )
        {
            if ( i == sv.size() - 1 )
            {
                str += sv[i];
            }else
            {
                str += sv[i] + ",";
            }
        }

        str += "}";
        return str;
    }
    void MapParam::for_each( eachfunc func )
    {
        if ( !func )
        {
            return;
        }

        for ( auto &p : _params )
        {
            func ( p.first, p.second );
        }
    }
}
