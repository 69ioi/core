#ifndef __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MANAGER_H_FILE__
#define __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MANAGER_H_FILE__
#include <ydPlugin/export.h>
#include <ydUtil/singleton.h>
#include <ydPlugin/Plugin.h>
#include <osg/ref_ptr>
#include <string>
#include <map>
#include <ydUtil/Param.h>

namespace ydPlugin
{
    class YDPLUGIN_API PluginManager : public Singleton<PluginManager>
    {
        friend class Singleton<PluginManager>;
    protected:
        PluginManager();
        ~PluginManager();
    public:
        void load( void *userData, std::string const& cfgFile = "plugin.xml" );
        void load( void *userData, ydUtil::VectorParam *lpParam );
        void unload();
        void setBinPath( std::string const& binPath );
        bool loadPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx, void* userData );
        bool unloadPlugin( std::string const& moudelName );

        Plugin* getPlugin(std::string const &moudelName);

        bool pluginKeyAction(unsigned int qtKey);

    protected:
        typedef std::map< std::string, osg::ref_ptr<Plugin> > PluginMap;
        PluginMap _pluginMap;
        std::string _pluginPath;

    };
}


#endif // __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MANAGER_H_FILE__
