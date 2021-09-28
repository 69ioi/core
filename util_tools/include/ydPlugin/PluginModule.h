#ifndef __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MODULE_H_FILE__
#define __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MODULE_H_FILE__

#include <ydPlugin/export.h>
#include <osg/Referenced>
#include <string>

namespace ydPlugin
{
  class YDPLUGIN_API PluginModule : public osg::Referenced
  {
  public:
    PluginModule( std::string const &name );

    virtual bool install( void* userData ){_userData = userData; return true;};
    virtual bool uninstall() = 0;
    virtual bool setup() = 0;
    virtual bool unsetup() = 0;
 
    std::string getName()const;

    virtual bool controlKey(unsigned int qtKey){ return false; };

    void *getUserData(){ return _userData; };

  private:
    std::string _name;
    void* _userData;

  };
  typedef PluginModule* (*CreatePluginModule)();
}


#endif // __YD_PLUGIN_INCLUDE_GR_PLUGIN_PLUGIN_MODULE_H_FILE__
