#ifndef S_SPTEXTURESAMPLER_H_
#define S_SPTEXTURESAMPLER_H_
#include "common.h"

class SPStudioManager;
class SPVulkanWindowsDriver;
class SPMaterial;
class SPMaterialVulkan;
class SPBuffer;
class  SPTextureSampler
{
public:
    friend class SPVulkanWindowsDriver;
    friend class SPMaterialVulkan;
    friend class SPMaterial;

    enum Result { Error = -1, Success = 0 };
    explicit SPTextureSampler() noexcept = default;
    SPTextureSampler(SPTextureSampler const& texture) = default;
    SPTextureSampler(SPTextureSampler && texture) noexcept = default;
    ~SPTextureSampler() noexcept = default;
    SPTextureSampler& operator=(SPTextureSampler & texture) = default;
    SPTextureSampler& operator=(SPTextureSampler && texture) noexcept = default;
    SPTextureSampler& id(const spstr & id) noexcept;
    SPTextureSampler& index(const spint & index) noexcept;
    SPTextureSampler& width(const spint & width) noexcept;
    SPTextureSampler& height(const spint & height) noexcept;
    SPTextureSampler& channel(const spint & channel) noexcept;
    SPTextureSampler& size(const spint & size) noexcept;
    SPTextureSampler& data(spuchar * &data) noexcept;
    Result create(SPStudioManager & manager, SPMaterial & material);
protected:
    spstr mId;
    spint mIndex;
    spint mWidth;
    spint mHeight;
    spint mChannel;
    spint mSize;
    spuchar* mData;
    SPBuffer* textureSamplerBuffer = nullptr;
};

#endif
