#ifndef __YD_PLUGIN_INCLUDE_GR_PLUGIN_WIN32_PLUGIN_H_FILE__
#define __YD_PLUGIN_INCLUDE_GR_PLUGIN_WIN32_PLUGIN_H_FILE__
#include <ydPlugin/Plugin.h>
#include <Windows.h>
namespace ydPlugin
{
    class OSPlugin : public Plugin
    {
        public:
            OSPlugin(std::string const& name, std::string const& pluginPath, std::string const& postfix);
			virtual bool load(void *userData);
            virtual bool unload();
        protected:
            HMODULE _dllHandle;

    };
}



#endif // __YD_PLUGIN_INCLUDE_GR_PLUGIN_WIN32_PLUGIN_H_FILE__
