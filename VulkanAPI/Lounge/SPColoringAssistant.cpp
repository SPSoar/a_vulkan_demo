#include "SPColoringAssistant.h"
#include "../SPMangaka.h"
#include "../Interface/SPDraft.h"
#include "../Resource/SPMaterial.h"
#include "../SPNeMu.h"
SPColoringAssistant::SPColoringAssistant(SPMangaka* boss)
	: boss(boss)
	, manager(boss->s_manager)
{
}

SPColoringAssistant::~SPColoringAssistant()
{
}

void SPColoringAssistant::purchaseMaterial(SPMaterialConfig& matConfig, SPNeMu* nemu)
{
	SPMaterial* mat = manager->createMaterial();
	SPTextureSampler()
		.id("0000")
		.index(0)
		.height(matConfig.texture.height)
		.width(matConfig.texture.width)
		.channel(matConfig.texture.channel)
		.size(matConfig.texture.size)
		.data(matConfig.texture.data)
		.create(*manager, *mat);

	SPColoringAssistant::Toning()
		.id(matConfig.id)
		.name(matConfig.name)
		.vertexShaderPath("D:/Desktop/Mditor/bin/Data/shaders/vert.spv")
		.fragmentShaderPath("D:/Desktop/Mditor/bin/Data/shaders/frag.spv")
		.tone(*manager, *mat);

	addMaterial(mat, nemu);
}


void SPColoringAssistant::coloring(SPDraft* model, SPNeMu* nemu)
{
	if (nemu->materials.find(model->id) != nemu->materials.end())
	{
		model->material = nemu->materials.find(model->id)->second;
	}
}

void SPColoringAssistant::coloringDraftByOneMaterial(const spstr& materialId, const spstr& draftId, SPNeMu* nemu)
{
	if (nemu->drafts.find(draftId) != nemu->drafts.end())
	{
		if (nemu->materials.find(nemu->drafts.find(draftId)->second->id) != nemu->materials.end())
		{
			nemu->drafts.find(draftId)->second->material = nemu->materials.find(nemu->drafts.find(draftId)->second->id)->second;
		}
	}
	
}

void SPColoringAssistant::destoryMaterial(const spstr& id, SPNeMu* nemu)
{
	if (nemu->materials.find(id) != nemu->materials.end())
	{
		manager->destroyShaderModule(nemu->materials[id]->vertexShaderBuffer);
		manager->destroyShaderModule(nemu->materials[id]->fragmentShaderBuffer);
		delete nemu->materials[id];
		nemu->materials.erase(id);
	}
}

void SPColoringAssistant::destoryAllMaterials(SPNeMu* nemu)
{
	std::unordered_map<spstr, SPMaterial*>::iterator iter;
	for (iter = nemu->materials.begin(); iter != nemu->materials.end(); iter++)
	{
		manager->destroyShaderModule(iter->second->vertexShaderBuffer);
		manager->destroyShaderModule(iter->second->fragmentShaderBuffer);
		delete iter->second;
	}
	nemu->materials.clear();
}

void SPColoringAssistant::destoryTexture(const spstr& id, SPNeMu* nemu)
{
	if (nemu->textures.find(id) != nemu->textures.end())
	{
		manager->destroyTextureSampler(nemu->textures[id]);
		nemu->textures.erase(id);
	}
}

void SPColoringAssistant::destoryAllTexture(SPNeMu* nemu)
{
	std::unordered_map<spstr, SPTextureSampler>::iterator iter;
	for (iter = nemu->textures.begin(); iter != nemu->textures.end(); iter++)
	{
		manager->destroyTextureSampler(iter->second);
	}
	nemu->textures.clear();
}



void SPColoringAssistant::addMaterial(SPMaterial* material, SPNeMu* nemu)
{
	if (material)
		nemu->materials[material->id] = material;
}

using BuilderType = SPColoringAssistant;

struct BuilderType::ToningDetails {
    using Skill = BuilderType::Toning::Skill;
	Skill mSkill;
    explicit ToningDetails(){}

};

BuilderType::Toning::Toning() noexcept
	: SPSkillBase<BuilderType::ToningDetails>()
{
}
BuilderType::Toning::~Toning() noexcept = default;
BuilderType::Toning::Toning(Toning&& coloring) noexcept = default;
BuilderType::Toning& BuilderType::Toning::operator=(BuilderType::Toning&& coloring) noexcept = default;

BuilderType::Toning& SPColoringAssistant::Toning::id(const spstr & id) noexcept
{
	mImpl->mSkill.mId = id;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::name(const spstr& name) noexcept
{
	mImpl->mSkill.mMaterialName = name;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::fileName(const spstr& name) noexcept
{
	mImpl->mSkill.mFileName = name;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::shading(Shading shading) noexcept
{
	mImpl->mSkill.mShading = shading;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::interpolation(Interpolation interpolation) noexcept
{
	mImpl->mSkill.mInterpolation = interpolation;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(UniformType type, Precision precision, const spstr& name) noexcept
{
	assert(mImpl->mSkill.mParameterCount < MAX_PARAMETERS_COUNT);
	mImpl->mSkill.mParameters[mImpl->mSkill.mParameterCount++] = { name, type, 1, precision };	
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(UniformType type, spint size, Precision precision, const spstr& name) noexcept
{
	assert(mImpl->mSkill.mParameterCount < MAX_PARAMETERS_COUNT); 
	mImpl->mSkill.mParameters[mImpl->mSkill.mParameterCount++] = { name, type, size, precision };
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SamplerType samplerType, SamplerFormat format, Precision precision, const spstr& name) noexcept
{
	assert(mImpl->mSkill.mParameterCount < MAX_PARAMETERS_COUNT);
	mImpl->mSkill.mParameters[mImpl->mSkill.mParameterCount++] = { name, samplerType, format, precision };
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SamplerType samplerType, SamplerFormat format, const spstr& name) noexcept
{
	return parameter(samplerType, format, Precision::SP_DEFAULT, name);
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SamplerType samplerType, Precision precision, const spstr& name) noexcept
{
	return parameter(samplerType, SamplerFormat::SP_FLOAT, precision, name);
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SamplerType samplerType, const spstr& name) noexcept
{
	return parameter(samplerType, SamplerFormat::SP_FLOAT, Precision::SP_DEFAULT, name);
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SubpassType subpassType, SamplerFormat format, Precision precision, const spstr& name) noexcept
{
	assert(format == SamplerFormat::SP_FLOAT);

	auto subpassCount = std::count_if(std::begin(mImpl->mSkill.mParameters), std::end(mImpl->mSkill.mParameters),
		[](const auto& p) { return p.isSubpass(); });
	assert(subpassCount < MAX_SUBPASS_COUNT);
	assert(mImpl->mSkill.mParameterCount < MAX_PARAMETERS_COUNT);
	mImpl->mSkill.mParameters[mImpl->mSkill.mParameterCount++] = { name, subpassType, format, precision };
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SubpassType subpassType, SamplerFormat format, const spstr& name) noexcept
{
	return parameter(subpassType, format, Precision::SP_DEFAULT, name);
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SubpassType subpassType, Precision precision, const spstr& name) noexcept
{
	return  parameter(subpassType, SamplerFormat::SP_FLOAT, precision, name);
}

BuilderType::Toning& SPColoringAssistant::Toning::parameter(SubpassType subpassType, const spstr& name) noexcept
{
	return parameter(subpassType, SamplerFormat::SP_FLOAT, Precision::SP_DEFAULT, name);
}


BuilderType::Toning& SPColoringAssistant::Toning::variable(Variable v, const spstr& name) noexcept
{
	switch (v) {
	case Variable::SP_CUSTOM0:
	case Variable::SP_CUSTOM1:
	case Variable::SP_CUSTOM2:
	case Variable::SP_CUSTOM3:
		assert(spint(v) < MATERIAL_VARIABLES_COUNT);
		mImpl->mSkill.mVariables[spint(v)] = name;
		break;
	}
	return *this;
}
//暂时这样定义 后续看一下这个函数作用
BuilderType::Toning& SPColoringAssistant::Toning::require(VertexAttribute attribute) noexcept
{
	mImpl->mSkill.mRequiredAttributes[attribute] = 1;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::materialDomain(MaterialDomain materialDomain) noexcept
{
	mImpl->mSkill.mMaterialDomain = materialDomain;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::vertexShaderPath(const spstr& filename) noexcept
{
	mImpl->mSkill.mVertexShaderPath = filename;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::fragmentShaderPath(const spstr& filename) noexcept
{
	mImpl->mSkill.mFragmentShaderPath = filename;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::quality(ShaderQuality quality) noexcept
{
	mImpl->mSkill.mShaderQuality = quality;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::blending(BlendingMode blending) noexcept
{
	mImpl->mSkill.mBlendingMode = blending;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::postLightingBlending(BlendingMode blending) noexcept
{
	mImpl->mSkill.mPostLightingBlendingMode = blending;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::vertexDomain(VertexDomain domain) noexcept
{
	mImpl->mSkill.mVertexDomain = domain;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::culling(CullingMode culling) noexcept
{
	mImpl->mSkill.mCullingMode = culling;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::colorWrite(spbool enable) noexcept
{
	mImpl->mSkill.mColorWrite = enable;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::depthWrite(spbool enable) noexcept
{
	mImpl->mSkill.mDepthWrite = enable;
	mImpl->mSkill.mDepthWriteSet = True;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::depthCulling(spbool enable) noexcept
{
	mImpl->mSkill.mDepthTest = enable;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::doubleSided(spbool doubleSided) noexcept
{
	mImpl->mSkill.mDoubleSided = doubleSided;
	mImpl->mSkill.mDoubleSidedCapability = True;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::maskThreshold(spfloat threshold) noexcept
{
	mImpl->mSkill.mMaskThreshold = threshold;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::shadowMultiplier(spbool shadowMultiplier) noexcept
{
	mImpl->mSkill.mShadowMultiplier = shadowMultiplier;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::transparentShadow(spbool transparentShadow) noexcept
{
	mImpl->mSkill.mTransparentShadow = transparentShadow;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::clearCoatIorChange(spbool clearCoatIorChange) noexcept
{
	mImpl->mSkill.mClearCoatIorChange = clearCoatIorChange;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::flipUV(spbool flipUV) noexcept
{
	mImpl->mSkill.mFlipUV = flipUV;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::multiBounceAmbientOcclusion(spbool multiBounceAO) noexcept
{
	mImpl->mSkill.mMultiBounceAO = multiBounceAO;
	mImpl->mSkill.mMultiBounceAOSet = True;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::specularAmbientOcclusion(SpecularAmbientOcclusion specularAO) noexcept
{
	mImpl->mSkill.mSpecularAO = specularAO;
	mImpl->mSkill.mSpecularAOSet = True;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::refractionMode(RefractionMode refraction) noexcept
{
	mImpl->mSkill.mRefractionMode = refraction;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::refractionType(RefractionType refractionType) noexcept
{
	mImpl->mSkill.mRefractionType = refractionType;
	return *this;
}

BuilderType::Toning& SPColoringAssistant::Toning::transparencyMode(TransparencyMode mode) noexcept
{
	mImpl->mSkill.mTransparencyMode = mode;
	return *this;
}

BuilderType::Toning::Result SPColoringAssistant::Toning::tone(SPStudioManager& manager, SPMaterial& material)
{
	//vertex
	material.vertexShaderBuffer = manager.createShaderModule(readShaderFile(mImpl->mSkill.mVertexShaderPath));
	material.fragmentShaderBuffer = manager.createShaderModule(readShaderFile(mImpl->mSkill.mFragmentShaderPath));
	return Result::Success;
}

std::vector<spchar> SPColoringAssistant::Toning::readShaderFile(const spstr& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
