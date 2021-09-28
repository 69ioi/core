#include <ydPlugin/PluginModule.h>

namespace ydPlugin
{
    PluginModule::PluginModule( std::string const&name )
        : _name( name )
    {
        _userData = NULL;
    }

    std::string PluginModule::getName()const
    {
        return _name;
    }
}
