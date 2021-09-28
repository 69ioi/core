#ifndef __YD_PLUGIN_INCLUDE_NF_PLUGIN_LINUX_PLUGIN_H_FILE__
#define __YD_PLUGIN_INCLUDE_NF_PLUGIN_LINUX_PLUGIN_H_FILE__
#include <ydPlugin/Plugin.h>
namespace ydPlugin
{
    class OSPlugin : public Plugin
    {
        public:
            OSPlugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx);
            virtual bool load(void *userData );
            virtual bool unload();
        protected:
            void *_dllHandle;

    };
}



#endif // __YD_PLUGIN_INCLUDE_NF_PLUGIN_LINUX_PLUGIN_H_FILE__
