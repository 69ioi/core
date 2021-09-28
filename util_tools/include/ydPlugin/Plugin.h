#ifndef __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_H_FILE__
#define __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_H_FILE__
#include <ydPlugin/export.h>
#include <osg/Referenced>
#include <ydPlugin/PluginModule.h>
#include <string>
#include <osg/ref_ptr>

namespace ydPlugin
{
    class YDPLUGIN_API Plugin : public osg::Referenced
    {
        public:
            Plugin( std::string const& pluginPath, std::string const& pluginFile, std::string const& postifx, std::string const& ext );
            //Plugin( std::string const&name, std::string const& path );
            bool isLoaded()const;
            void setName(std::string const& name);
            std::string getName()const;

            // other plugin must replay impl this function
            virtual bool load( void *userData );
            virtual bool unload();

            PluginModule *getModule();

            void *getUserData();

        protected:
            bool _loaded;
            std::string _name;
            std::string _path;

            osg::ref_ptr<PluginModule> _pluginModule;

            void *_userData;

    };
}


#endif // __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_H_FILE__
