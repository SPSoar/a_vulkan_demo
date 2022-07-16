#ifndef S_SPPLUGINFACTORY_H_
#define S_SPPLUGINFACTORY_H_


#define PLUGIN_FACTORY_CREATE "Create_Plugin_Facade"

#define PLUGIN_FACTORY_DESTROY "Destroy_Plugin_Facade"




#define PLUGIN_FACTORY_DECLARATION(T)                   \
    extern "C"                                          \
    {                                                   \
        PLUGINEXPORT T* Create_Plugin_Facade();             \
        PLUGINEXPORT void Destroy_Plugin_Facade();          \
    }


#define PLUGIN_FACTORY_DEFINITION(T)        \
    T* Global_Instance_ = nullptr;          \
    T* Create_Plugin_Facade()               \
    {                                       \
        if (!Global_Instance_)              \
            Global_Instance_ = new T();     \
        return Global_Instance_;            \
    }                                       \
    void Destroy_Plugin_Facade()            \
    {                                       \
        if (Global_Instance_)               \
        {                                   \
            delete Global_Instance_;        \
            Global_Instance_ = nullptr;     \
        }                                   \
    }

#endif  