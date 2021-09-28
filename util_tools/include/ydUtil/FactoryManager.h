#ifndef __YDUTIL_FACTORY_MANAGER_H_FILE__
#define __YDUTIL_FACTORY_MANAGER_H_FILE__
#include <osg/Referenced>
#include <iostream>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <string>
#include <map>
#include <vector>


template<class T, class MT>
class FactoryManager
{
protected:
    FactoryManager()
    {

    }
    ~FactoryManager()
    {
    }
public:
    template<class T2, class MT2>
        class Factory : public osg::Referenced
    {
    public:
        Factory<T2, MT2>( MT2 const& identify )
            : _identify( identify )

        {
            FactoryManager<T2, MT2>::Instance()->addFactory( this );
        }

        MT2 getIdentify(){return _identify; };

        virtual T2 *create()=0;

    protected:
        MT2 _identify;
    };
    static FactoryManager<T, MT > *Instance( bool destory = false )
    {
        static FactoryManager<T, MT> *S_Manager = NULL;
        if ( destory )
        {
            delete S_Manager;
            S_Manager = NULL;

            return NULL;
        }
        if ( S_Manager )
        {
            return S_Manager;
        }
        S_Manager = new FactoryManager<T, MT>;
        return S_Manager;
   }
    static void Destory()
    {
        Instance( true );
    }


    void addFactory( Factory<T, MT> *lpFactory )
    {
        if ( lpFactory )
        {
            _tlist[lpFactory->getIdentify()] = lpFactory;
        }
    }
    T *create( MT const& identify )
    {
        if ( _tlist.find( identify ) == _tlist.end() )
        {
            std::cout << "unknow identify : " << identify << std::endl;
            return NULL;
        }
        return _tlist[identify]->create();
    }

    protected:
        typedef std::map<MT, osg::ref_ptr<Factory<T, MT> > > TList;
        TList _tlist;
    public:
        typedef std::vector<MT> TIdentifyList;

        TIdentifyList getIdentifyList()
        {
            TIdentifyList ti;
            for ( typename TList::iterator it = _tlist.begin(); it != _tlist.end(); it ++ )
            {
                ti.push_back( it->first );
            }
            return ti;
        }

};

#define COMMON_FACTORY(className, classID, baseclass, identifyType)   \
    class className##Factory : public FactoryManager<baseclass, identifyType>::Factory<baseclass, identifyType> \
    {                                                                   \
    public:                                                             \
    className##Factory( identifyType const& identify )                  \
        :FactoryManager<baseclass, identifyType>::Factory<baseclass, identifyType>(identify) \
        {                                                               \
        };                                                              \
        virtual baseclass *create(){ className *lpC = new className; return lpC; }; \
    };                                                                  \
    osg::observer_ptr<className##Factory> g_##className##Factory = new className##Factory( #classID );


#define DEF_FACTORYMANAGER(baseclass, identifytype, exportFlag ) \
class exportFlag baseclass##FactoryManager : public FactoryManager< baseclass, identifytype >\
{\
\
};
#define DECLARE_FACTORYMANAGER(baseclass, identifytype, exportFlag) class exportFlag baseclass##FactoryManager;

#endif // __YDUTIL_FACTORY_MANAGER_H_FILE__
