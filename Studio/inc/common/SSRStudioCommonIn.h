#pragma once
#ifndef SSR_SSRSTUDIOCOMMONIN_H_
#define SSR_SSRSTUDIOCOMMONIN_H_

_SSR_STUDIO_BEGIN

using SC_BindingSet        = SSRFixVector<_SSR_CORE::SSRBindingSetPtr, 3>;
using SC_BindingLayout     = SSRFixVector<_SSR_CORE::SSRBindingLayoutPtr, 3>;
using SC_GraphicsPipeline  = SSRFixVector<_SSR_CORE::SSRGraphicsPipelinePtr, 3>;
using SC_SubDrawCmd        = SSRFixVector<_SSR_CORE::SSRSubDrawCommandPtr, 3>;
using SC_SubTransferCmd    = SSRFixVector<_SSR_CORE::SSRSubTransferCommandPtr, 3>;

// <=3
enum class SE_PassUsage : SENUM8
{
    eColor,
    ePostProcessing,
    ePreProcessing,
    eOther,
};

struct SI_PassParams
{
    union
    {
        struct Param
        {
            SE_PassUsage  usage       : 2;
            SBOOL         depthEnable : 1;
            SBOOL         depthWrite  : 1;
            SBOOL         colorWrite  : 1;
            SUINT8        padding0 : 1;
            SUINT8        padding1 : 4;
            SUINT8        padding2 : 8;
        }       p;
        SUINT32 u;
    };
    SI_PassParams() noexcept 
    {
        p.usage = SE_PassUsage::eColor;
        p.depthEnable = true;
        p.depthWrite = true;
        p.colorWrite = true;
    }
private:
    friend inline bool operator < (SI_PassParams lhs, SI_PassParams rhs)
    {
        return lhs.u < rhs.u;
    }
};

_SSR_SASSERT(sizeof(SI_PassParams) == sizeof(SUINT32), "SamplerParams must be 32 bits");



#define  _SSR_FLAGS_COMBINATION_MATERIAL_VARIANT_DEF(_SSR_DEFINER)\

_SSR_FLAGS_DEF(SE_MaterialVariant,
               _SSR_FLAGS_COMBINATION_MATERIAL_VARIANT_DEF(_SSR_FLAGS_COMBINATION_DEF),
               Ssao, ssao,
               Color, color,
               DepthOnly, depth_only);

using MaterialVariantKey = SUINT32;

struct SI_PassInfo
{
    SI_PassParams           mConfig{};
    SE_MaterialVariantFlags mMaterialVariant{};
};

inline constexpr auto S_CF_Convert_Set_To_Layout = [](const  _SSR_CORE::SSRBindingSetItemVector& setDesc, _SSR_CORE::SSRBindingLayoutItemVector& layoutDesc)
        {
            for (const auto& item : setDesc)
            {
                _SSR_CORE::SSRBindingLayoutItem layoutItem{};
                layoutItem.mSlot = item.mSlot;
                layoutItem.mType = item.mType;
                if (item.mType == _SSR_CORE::RE_ResourceType::eUniformBuffer)
                {
                    layoutItem.mSize = cast<uint32_t>(item.mRange.mByteSize);
                }
                layoutDesc.push_back(layoutItem);
            }
        };

_SSR_END

#endif

