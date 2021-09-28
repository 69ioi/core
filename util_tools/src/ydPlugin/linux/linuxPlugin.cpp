#include "linuxPlugin.h"
#include <ydPlugin/PluginModule.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <ydUtil/log.h>

namespace ydPlugin
{
    OSPlugin::OSPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx)
   // OSPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx)
        : Plugin( pluginPath, "lib" + pluginFile, postifx, "so" )
    {
        _name = pluginFile;
        _dllHandle = NULL;
    }
    bool OSPlugin::load(void *userData)
    {
        Plugin::load( userData );

        _dllHandle = dlopen(_path.c_str(), RTLD_LAZY );
        if (!_dllHandle)
        {
            TRACELL(LL_ERROR, "Open Error:%s.\n",dlerror());
            return false;
        }

        CreatePluginModule cpmfun;
        cpmfun = (CreatePluginModule)dlsym(_dllHandle,"CreateModule");
        const char *errorInfo = dlerror();
        if (errorInfo != NULL){
            TRACELL(LL_ERROR, "Dlsym Error:%s.\n",errorInfo);
            return false;
        }

        if ( !cpmfun )
        {
            TRACELL(LL_ERROR, "get CreateModule failed\n");
            return false;
        }

        PluginModule *lpPluginModule = cpmfun();
        if ( !lpPluginModule )
        {
            TRACELL(LL_ERROR, "call CreateModule failed\n");
            return false;
        }
        lpPluginModule->install( userData );

        lpPluginModule->setup();
        _pluginModule = lpPluginModule;


        return true;
    }

    bool OSPlugin::unload()
    {
        Plugin::unload();

        if ( _pluginModule )
        {
            _pluginModule->uninstall();
            _pluginModule = NULL;
        }

        dlclose(_dllHandle);
        _dllHandle = NULL;

        // todo 
        return true;
    }
}
