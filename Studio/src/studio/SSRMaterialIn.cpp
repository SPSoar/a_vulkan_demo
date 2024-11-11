#include "studio/SSRMaterialIn.h"
#include "studio/SSRMaterialInstanceIn.h"
#include "studio/SSRStudioIn.h"

#include <Shaders/SSRMaterialBucket.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define _SSR_MATERIAL_SHADER_FACTORY_DEF(Type, Variant, Data, Size)\
    {auto key = Make_Material_Key(Type, Variant);\
    auto& shader = mShaderMap[key]; \
    info.mShaderType = Type;\
    info.mData = Data;\
    info.mSize = Size * sizeof(uint32_t); \
    info.mShaderName = "main"; \
    shader = mStudio.get_token().make_shader(_SSR_STD::move(info));\
    _SSR_ASSERT(shader != nullptr, _SSR_RDERROR);}\
    _SSR_ASSERT(Data,_SSR_RDERROR)

_SSR_STUDIO_BEGIN

_SSR_SHARED_IMPL_IN(SSRMaterial)

_SSR_MATERIAL_PUB_MEMBER_FUNC_IMPL(SSRMaterial)

namespace 
{
    inline constexpr auto CIF_App_Material_Type_To_Studio_Type =
        [](const _SSR_DATA::SSRDMaterial::E_MaterialType type) -> _SSR_STUDIO::E_Shading
        {
            switch (type)
            {
            case _SSR_DATA::SSRDMaterial::eRectangle3D:       return _SSR_STUDIO::E_Shading::eSurface;
            case _SSR_DATA::SSRDMaterial::eLine3D:            return _SSR_STUDIO::E_Shading::eLine;
            case _SSR_DATA::SSRDMaterial::eNone:
            default: _SSR_STD::unreachable();
            }
        };
}

auto SSRMaterial::Create(SSRStudioRef studio) -> SSRMaterialSPtr
{
    return _SSR_MAKE_SHARED_IN(SSRMaterial, studio);
}

auto SSRMaterialIn::Make_Material_Key(const _SSR_CORE::RE_ShaderType shaderType, const SE_MaterialVariantFlags::ValueType variant)  noexcept -> MaterialVariantKey
{
    _SSR_SASSERT(sizeof(_SSR_CORE::RE_ShaderType) == 2, _SSR_RDERROR);
    _SSR_SASSERT(sizeof(SE_MaterialVariantFlags::ValueType)  == 2, _SSR_RDERROR);
    return cast<MaterialVariantKey>((cast<SUINT32>(shaderType) << 16) | variant);
}

SSRMaterialIn::SSRMaterialIn(SSRStudioRef studio) noexcept
    : mStudio{ upcast<SSRStudioInRef>(studio) }
{

}

SSRMaterialIn::~SSRMaterialIn() noexcept
{
    for (auto it = mShaderMap.begin(); it != mShaderMap.end(); ++it)
    {
        if (auto& shader = it.value())
        {
            mStudio.get_token().delete_shader(shader);
            shader = nullptr;
        }
    }
    mShaderMap.clear();
    _SSR_DELETE_OBJECT_PTR(mTextureGroup);
}

auto SSRMaterialIn::init() noexcept -> SSRMaterial&
{
    return *this;
}
auto SSRMaterialIn::init_name(const char* name) noexcept -> SSRMaterial&
{
    return *this;
}

auto SSRMaterialIn::setup() noexcept -> SSRMaterial&
{
    return *this;
}
auto SSRMaterialIn::setup_package(_SSR_DATA::SSRDMaterial::Material* material) noexcept -> SSRMaterial&
{
    mMaterialDesc = material;
    return *this;
}
auto SSRMaterialIn::commit() noexcept -> void
{
    using Key = _SSR_DATA::SSRDMaterial::Key;

    _SSR_ASSERT(mMaterialDesc, _SSR_RDERROR);

    mShading = CIF_App_Material_Type_To_Studio_Type(
               cast<_SSR_DATA::SSRDMaterial::E_MaterialType>(_SSR_DGET((*mMaterialDesc), Key::eType)));

     _SSR_ASSERT(mShading != E_Shading::eNone, _SSR_RDERROR);

    if (mShading == E_Shading::eSurface) 
    {
        _SSR_CORE::RI_ShaderInfo info{};
        
        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eVertex,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_SURFACE_VERT_DATA,
                                         _SSR_SURFACE_VERT_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eFragment,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_GOOCH_FRAG_DATA,
                                         _SSR_GOOCH_FRAG_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eFragment,
                                         SE_MaterialVariantFlags::eDepthOnly,
                                         _SSR_DEPTHONLYSURFACE_FRAG_DATA,
                                         _SSR_DEPTHONLYSURFACE_FRAG_SIZE);
    }
    else if (mShading == E_Shading::eLine) 
    {
        _SSR_CORE::RI_ShaderInfo info{};

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eVertex,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_WIREFRAME_VERT_DATA,
                                         _SSR_WIREFRAME_VERT_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eTessellationControl,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_WIREFRAME_TESC_DATA,
                                         _SSR_WIREFRAME_TESC_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eTessellationEvaluation,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_WIREFRAME_TESE_DATA,
                                         _SSR_WIREFRAME_TESE_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eGeometry,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_WIREFRAME_GEOM_DATA,
                                         _SSR_WIREFRAME_GEOM_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eFragment,
                                         SE_MaterialVariantFlags::eColor,
                                         _SSR_SINGLECOLOR_FRAG_DATA,
                                         _SSR_SINGLECOLOR_FRAG_SIZE);

        _SSR_MATERIAL_SHADER_FACTORY_DEF(_SSR_CORE::RE_ShaderType::eFragment,
                                         SE_MaterialVariantFlags::eDepthOnly,
                                         _SSR_DEPTHONLYWIREFRAME_FRAG_DATA,
                                         _SSR_DEPTHONLYWIREFRAME_FRAG_SIZE);
    }

    parse();

    mShaderCompiled = true;
}

auto SSRMaterialIn::prepare() noexcept -> void
{
    _SSR_UNLIKELY_IF(!mShaderCompiled)
    {
        commit();
    }

    if (mBuildFinished.test(_SSR_STD::memory_order_relaxed))
    {
        return;
    }

    // prepare 主线程完成，不需要定序
    mBuildFinished.clear(_SSR_STD::memory_order_relaxed);
    
    mTextureGroup->prepare();
}

auto SSRMaterialIn::build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void
{
    // prepare 多线程，只有第一次进入时 buil 
    if (mBuildFinished.test_and_set(_SSR_STD::memory_order_seq_cst))
    {
        return;
    }

    mTextureGroup->build(transferCommand);
}

auto SSRMaterialIn::parse() noexcept -> void
{
    using Uid  = _SSR_DATA::SSRDMaterial::Key;
    using Type = _SSR_DATA::SSRDMaterial::Value;

    _SSR_UNUSED_P(Uid::eBaseColor);

    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor), 1, Type::eBaseColor);

    mSamplerInterfaceBlock.add_sampler(_SSR_EXPAND_STR(Uid::eBaseColor), SSRSamplerInterfaceBlock::Type::eSampler2d);
    _SSR_CREATE_OBJECT_PTR(mTextureGroup, SSRTextureGroup, mSamplerInterfaceBlock.size());

    const auto& cfg = _SSR_CORE::SSRCoreConfig::Config();
    auto&& appResourceDir = cfg.get_material().at(_SSR_CORE::E_ConfigCode::eAppResourceDir);
    auto patch = _SSR_STDF::path(appResourceDir).append("BaseColor.png").make_preferred();
    int w, h, c;
    unsigned char* idata = stbi_load(patch.string().c_str(), &w, &h, &c, 0);

    _SSR::SSRRawImage* img = new _SSR::SSRRawImage();
    img->mWidth = cast<SUINT32>(w);
    img->mHeight = cast<SUINT32>(h);
    img->mChannel = cast<SUINT32>(c);
    img->mData = idata;

    auto texture = SSRTexture::Create(mStudio);
    texture->init()
        .init_name("haha")
        .setup()
        .setup_raw_image(img)
        .commit();

    _SSR_STUDIO_TEXTURE_SET(mSamplerInterfaceBlock, mTextureGroup, Uid::eBaseColor, texture);
}

auto SSRMaterialIn::make_instance() noexcept -> SSRMaterialInstanceSPtr
{
    return _SSR_MAKE_SHARED_IN(SSRMaterialInstance, shared_from_this());
}

_SSR_END