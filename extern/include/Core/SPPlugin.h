#ifndef S_SPPLUGIN_H_
#define S_SPPLUGIN_H_

#include <Base/base_common.h>

class COREEXPORT SPPlugin
{
public:

    virtual const sp_string& get_plugin_name() const = 0;
    virtual const sp_string& get_plugin_version() const = 0;
    virtual void init() = 0;
    virtual void dispose() = 0;
    virtual void update() = 0;

protected:
    virtual ~SPPlugin() = default;
};

#endif  