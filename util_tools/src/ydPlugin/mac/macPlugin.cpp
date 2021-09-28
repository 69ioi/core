#include "macPlugin.h"
#include <ydPlugin/PluginModule.h>
#include <dlfcn.h>

namespace ydPlugin
{
    OSPlugin::OSPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx)
   // LinuxPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx)
        : Plugin( pluginPath, "lib" + pluginFile, postifx, "dylib" )
    {
        _name = pluginFile;
        _dllHandle = NULL;
    }
    bool OSPlugin::load( void* userData )
    {
        Plugin::load( userData );

        _dllHandle = dlopen(_path.c_str(), RTLD_LAZY );
        if (!_dllHandle)
        {
            printf("Open Error:%s.\n",dlerror());
            return 0;
        }

        CreatePluginModule cpmfun;
        cpmfun = (CreatePluginModule)dlsym(_dllHandle,"CreateModule");
        const char *errorInfo = dlerror();
        if (errorInfo != NULL){
            printf("Dlsym Error:%s.\n",errorInfo);
            return 0;
        }


        PluginModule *lpPluginModule = cpmfun();
        if ( !lpPluginModule )
        {
            return false;
        }
        lpPluginModule->install( userData );

        _pluginModule = lpPluginModule;
        setName( _pluginModule->getName() );
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
