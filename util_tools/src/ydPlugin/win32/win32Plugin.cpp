#include "win32Plugin.h"
#include <ydPlugin/PluginModule.h>

namespace ydPlugin
{
    OSPlugin::OSPlugin(std::string const& name, std::string const& pluginPath, std::string const& postfix)
        : Plugin( name, pluginPath, postfix, "dll" )
    {

        _name = pluginPath;
        _dllHandle = NULL;
    }
    bool OSPlugin::load(void *userData)
    {
        Plugin::load(userData);


        CreatePluginModule cpmfun;

        HMODULE pluginHandle = LoadLibraryA(_path.c_str());

        if ( !pluginHandle )
        {
            DWORD err = GetLastError();
            fprintf(stderr, "load %s err : %d\n", _path.c_str(), err );
            return false;
        }

        _dllHandle = pluginHandle;
        cpmfun = (CreatePluginModule)GetProcAddress(pluginHandle, "CreateModule");
        if (!cpmfun)
        {
            DWORD err = GetLastError();
            fprintf(stderr, "can't find the function : \"CreateModule\" from %s, errno : %d\n", _path.c_str(), err);
            return false;
        }

        PluginModule *lpPluginModule = cpmfun( );
        if ( !lpPluginModule )
        {
            return false;
        }
        lpPluginModule->install( userData );
        setName(lpPluginModule->getName());

        _pluginModule = lpPluginModule;
		lpPluginModule->setup();
        return true;
    }

    bool OSPlugin::unload()
    {
        Plugin::unload();

        if ( _pluginModule )
        {
            _pluginModule->uninstall();
            //delete _pluginModule;
            _pluginModule = NULL;
        }

        FreeLibrary( _dllHandle );
        _dllHandle = NULL;

        // todo 
        return true;
    }
}
