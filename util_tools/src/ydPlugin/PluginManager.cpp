#include <ydPlugin/PluginManager.h>
#include <ydPlugin/Plugin.h>
#include <ydUtil/log.h>

#ifdef WIN32
#include "win32/win32Plugin.h"
#else
#ifdef __APPLE__
#include "mac/macPlugin.h"
#else
#include "linux/linuxPlugin.h"
#endif
#endif

#include <tinyxml2.h>

namespace ydPlugin
{
    PluginManager::PluginManager()
    {
    }

    PluginManager::~PluginManager()
    {
    }
    void PluginManager::load( void *userData, ydUtil::VectorParam *lpParam )
    {
        if ( !lpParam )
        {
            return;
        }
        std::string postifx = "";
#if defined(_DEBUG) || defined(DEBUG)
        postifx = "d";
#endif

        lpParam->for_each( [this, postifx, userData](ydUtil::Param *inParam ){
                               if ( !inParam )
                               {
                                   return;
                               }
                               std::string fileName = inParam->value();
                               if ( !loadPlugin( this->_pluginPath, fileName, postifx, userData ) )
                               {
                                   TRACELL(LL_ERROR, "load %s%s failed\n", fileName.c_str(), postifx.c_str());
                               }else
                               {
                                   TRACELL(LL_INFO, "load %s%s ok\n", fileName.c_str(), postifx.c_str());
                               }
                           });
    }
    /*
    void PluginManager::load( void *userData, std::string const& pluginFile )
    {
        std::string cfgFile = _pluginPath + "/" + pluginFile;
        std::string postifx = "";
#if defined(_DEBUG) || defined(DEBUG)
        postifx = "d";
#endif
        tinyxml2::XMLDocument doc;
        doc.LoadFile(cfgFile.c_str());

        tinyxml2::XMLElement* pluginsElement = doc.FirstChildElement("plugins");
        if ( !pluginsElement )
        {
            return;
        }

        tinyxml2::XMLElement *pluginElement = pluginsElement->FirstChildElement("plugin");
        while ( pluginElement )
        {
            if ( pluginElement->Attribute("file") )
            {
                std::string pluginName = pluginElement->Attribute("file");
                if (pluginName.length() > 0)
                {
                    std::string fileName = pluginElement->Attribute("file");
                    if ( !loadPlugin( _pluginPath, fileName, postifx, userData ) )
                    {
                        TRACELL(LL_ERROR, "load %s%s failed\n", fileName.c_str(), postifx.c_str());
                    }else
                    {
                        TRACELL(LL_INFO, "load %s%s ok\n", fileName.c_str(), postifx.c_str());
                    }
                }
            }
            pluginElement = pluginElement->NextSiblingElement( "plugin" );
        }
    }
    */
    void PluginManager::setBinPath( std::string const& binPath )
    {
        _pluginPath = binPath + "/plugins";
    }

    bool PluginManager::loadPlugin( std::string const& pluginPath, std::string const& pluginName, std::string const& postifx, void* userData )
    {
        osg::ref_ptr<Plugin> lpPlugin = NULL;

        lpPlugin = new OSPlugin( pluginPath, pluginName, postifx );

        if ( lpPlugin )
        {
            if ( lpPlugin->load( userData ) )
            {
                _pluginMap[lpPlugin->getName()] = lpPlugin;
                return true;
            }
        }

        return false;
    }

    bool PluginManager::unloadPlugin( std::string const& moduleName )
    {
        PluginMap::iterator it = _pluginMap.find( moduleName );
        if ( it == _pluginMap.end() )
        {
            return false;
        }

        if ( (*it).second.valid() )
        {
            (*it).second->unload();
            _pluginMap.erase( it );
            return true;
        }

        return false; 
    }

    Plugin *PluginManager::getPlugin(std::string const &moudelName)
    {
        if (_pluginMap.find(moudelName) == _pluginMap.end())
        {
            return NULL;
        }

        return _pluginMap[moudelName].get();
    }

    void PluginManager::unload()
    {
        while (_pluginMap.size() > 0)
        {
            PluginMap::iterator it = _pluginMap.begin();
            unloadPlugin(it->first);
        }
    }

    bool PluginManager::pluginKeyAction(unsigned int qtKey)
    {
        for (PluginMap::iterator it = _pluginMap.begin(); it != _pluginMap.end(); it++)
        {
            if (it->second.valid() && it->second->getModule() )
            {
                if (it->second->getModule()->controlKey(qtKey))
                {
                    return true;
                }
            }
        }

        return false;
    }
}
