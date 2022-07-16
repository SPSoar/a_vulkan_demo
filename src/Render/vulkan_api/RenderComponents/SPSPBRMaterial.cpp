#include "SPSPBRMaterial.h"


SPSPBRMaterial::SPSPBRMaterial(const std::string& name)
    : SPComponent{ name }
{
}

std::type_index SPSPBRMaterial::get_type()
{
    return typeid(SPSPBRMaterial);
}
