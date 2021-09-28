#include <ydPlugin/Plugin.h>

namespace ydPlugin
{
    Plugin::Plugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx, std::string const& ext )
    {
        _loaded = false;
        _userData = NULL;
        
#ifdef WIN32
        _path = pluginPath + "\\" + pluginFile + postifx + "." + ext;
#else 
        _path = pluginPath + "/" + pluginFile + postifx + "." + ext;
#endif
        _name = pluginFile;
        //printf("%s:%d->%s\n", __FILE__, __LINE__, _name.c_str());
    }

    bool Plugin::isLoaded()const
    {
        return _loaded;
    }

    bool Plugin::load( void *userData )
    {
        _loaded = true;
        _userData = userData;
        return true;
    }

    bool Plugin::unload()
    {
        _loaded = false;

        return true;
    }

    void Plugin::setName(std::string const& name)
    {
        _name = name;
    }

    std::string Plugin::getName()const
    {
        return _name;
    }

    PluginModule *Plugin::getModule()
    {
        return _pluginModule.get();
    }
    void *Plugin::getUserData()
    {
        return _userData;
    }
}
