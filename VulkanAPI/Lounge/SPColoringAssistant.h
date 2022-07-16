#ifndef S_SPCOLORINGASSISTANT_H_
#define S_SPCOLORINGASSISTANT_H_
#include "common.h"
#include "../SPBase.h"
#include "../Resource/SPTextureSampler.h"
class SPMangaka;
class SPStudioManager;
class SPMaterial;
class SPNeMu;
class SPDraft;
class SPDrawingPadVulkan;
class RHIEXPORT SPColoringAssistant
{
    struct ToningDetails;
	friend SPNeMu;
	friend SPDrawingPadVulkan;
   
public:
	SPColoringAssistant(SPMangaka* boss);
	virtual ~SPColoringAssistant();
	void purchaseMaterial(SPMaterialConfig& matConfig, SPNeMu* nemu);
	void coloring(SPDraft* model, SPNeMu* nemu);
	void coloringDraftByOneMaterial(const spstr& materialId, const spstr& draftId, SPNeMu* nemu);

    void destoryMaterial(const spstr& id, SPNeMu* nemu);
    void destoryAllMaterials(SPNeMu* nemu);
    void destoryTexture(const spstr& id, SPNeMu* nemu);
    void destoryAllTexture(SPNeMu* nemu);


    class Toning : public SPSkillBase<ToningDetails>
    {
        friend struct ToningDetails;
    public:
        enum Result { Error = -1, Success = 0 };
        static constexpr spint MATERIAL_VARIABLES_COUNT = 4;
        enum class Variable : spuint {
             SP_CUSTOM0,
             SP_CUSTOM1,
             SP_CUSTOM2,
             SP_CUSTOM3
            // when adding more variables, make sure to update MATERIAL_VARIABLES_COUNT
        };

        enum class MaterialDomain : spuint {
            SP_SURFACE = 0, 
            SP_POST_PROCESS = 1
        };


        enum class RefractionMode : spuint {
            SP_NONE = 0,
            SP_CUBEMAP = 1,
            SP_SCREEN_SPACE = 2, 
        };
        
        enum class RefractionType : spuint {
            SP_SOLID = 0,
            SP_THIN = 1, 
        };

        enum class ShaderQuality : spint {
            SP_DEFAULT = -1,   
            SP_LOW = 0,    
            SP_NORMAL = 1,   
            SP_HIGH = 2   
        };

        enum class BlendingMode : spuint {
           SP_OPAQUE,
           SP_TRANSPARENT,
           SP_ADD,
           SP_MASKED,
           SP_FADE,
           SP_MULTIPLY,
           SP_SCREEN,
        };
        //������ڸ�Ϊ�Լ���Ҫ������
        enum class Shading : spuint {
            SP_UNLIT,                
            SP_LIT,
            SP_SUBSURFACE,
            SP_CLOTH,
            SP_SPECULAR_GLOSSINESS,
        };
        //��ֵ
        enum class Interpolation : spuint {
            SP_SMOOTH,
            SP_FLAT
        };

        enum class VertexDomain : spuint {
             SP_OBJECT,                
             SP_WORLD,                 
             SP_VIEW,                   
             SP_DEVICE                 
        };

        /**
         * Vertex attribute types
         */
        enum VertexAttribute : spuint {

            SP_POSITION = 0, 
            SP_TANGENTS = 1, 
            SP_COLOR = 2,
            SP_UV0 = 3, 
            SP_UV1 = 4,
            SP_BONE_INDICES = 5,
            SP_BONE_WEIGHTS = 6,
          };

        //͸����
        enum class TransparencyMode : spuint {
            SP_DEFAULT,
            SP_TWO_PASSES_ONE_SIDE,
            SP_TWO_PASSES_TWO_SIDES
        };

        enum class SpecularAmbientOcclusion : spuint {
            SP_NONE = 0, 
            SP_SIMPLE = 1, 
            SP_BENT_NORMALS = 2, 
        };

        //-------------------------------------------
        enum class UniformType : spuint {
            SP_BOOL,
            SP_BOOL2,
            SP_BOOL3,
            SP_BOOL4,
            SP_FLOAT,
            SP_FLOAT2,
            SP_FLOAT3,
            SP_FLOAT4,
            SP_INT,
            SP_INT2,
            SP_INT3,
            SP_INT4,
            SP_UINT,
            SP_UINT2,
            SP_UINT3,
            SP_UINT4,
            SP_MAT3,  
            SP_MAT4    
        };

        enum class SamplerType : spuint {
           SP_SAMPLER_2D,        
           SP_SAMPLER_2D_ARRAY,   
           SP_SAMPLER_CUBEMAP,    
           SP_SAMPLER_EXTERNAL,  
           SP_SAMPLER_3D,        
        };

        enum class SubpassType : spuint {
            SP_SUBPASS_INPUT
        };

        enum class SamplerFormat : spuint {
            SP_INT = 0,        
            SP_UINT = 1,      
            SP_FLOAT = 2,      
            SP_SHADOW = 3    
        };
        //����
        enum class Precision : spuint {
            SP_LOW,
            SP_MEDIUM,
            SP_HIGH,
            SP_DEFAULT
        };

        enum class CullingMode : spuint {
            SP_NONE,              
            SP_FRONT,              
            SP_BACK,               
            SP_FRONT_AND_BACK     
        };

        enum class OutputTarget : spuint {
            SP_COLOR,
            SP_DEPTH
        };

        enum class OutputType : spuint {
            SP_FLOAT,
            SP_FLOAT2,
            SP_FLOAT3,
            SP_FLOAT4
        };

       
        explicit Toning() noexcept;
        Toning(Toning const& coloring) = delete;
        Toning(Toning&& coloring) noexcept;
        ~Toning() noexcept;
        Toning& operator=(Toning& coloring) = delete;
        Toning& operator=(Toning&& coloring) noexcept;
        Toning& id(const spstr& id) noexcept;
        //��������
        Toning& name(const spstr& name) noexcept;
        //������ļ�����
        Toning& fileName(const spstr& name) noexcept;
        //��ɫ����
        Toning& shading(Shading shading) noexcept;
        //��ֵ����
        Toning& interpolation(Interpolation interpolation) noexcept;
        Toning& parameter(UniformType type, Precision precision, const spstr& name) noexcept;
        Toning& parameter(UniformType type, const spstr& name) noexcept {
            return parameter(type, Precision::SP_DEFAULT, name);
        }
        Toning& parameter(UniformType type, spint size,
            Precision precision, const spstr& name) noexcept;
        Toning& parameter(UniformType type, spint size, const spstr& name) noexcept {
            return parameter(type, size, Precision::SP_DEFAULT, name);
        }
        Toning& parameter(SamplerType samplerType, SamplerFormat format,
            Precision precision, const spstr& name) noexcept;
        Toning& parameter(SamplerType samplerType, SamplerFormat format,
            const spstr& name) noexcept;
        Toning& parameter(SamplerType samplerType, Precision precision,
            const spstr& name) noexcept;
        Toning& parameter(SamplerType samplerType, const spstr& name) noexcept;
        
        
        /**
        * Add a subpass parameter to this material.
        */
        Toning& parameter(SubpassType subpassType, SamplerFormat format, Precision precision, const spstr& name) noexcept;
        Toning& parameter(SubpassType subpassType, SamplerFormat format, const spstr& name) noexcept;
        Toning& parameter(SubpassType subpassType, Precision precision,  const spstr& name) noexcept;
        Toning& parameter(SubpassType subpassType, const spstr& name) noexcept;
        
        //����������
        Toning& variable(Variable v, const spstr& name) noexcept;
        Toning& require(VertexAttribute attribute) noexcept;
        Toning& materialDomain(MaterialDomain materialDomain) noexcept;
        //������ʹ���������ݣ�����д��
        Toning& material(const char* code, spint line = 0) noexcept;
        Toning& materialVertex(const char* code, size_t line = 0) noexcept;

        Toning& vertexShaderPath(const spstr& filename) noexcept;
        Toning& fragmentShaderPath(const spstr& filename) noexcept;

        //��ʱ������callback
        //Toning& includeCallback(IncludeCallback callback) noexcept;
        Toning& quality(ShaderQuality quality) noexcept;
        Toning& blending(BlendingMode blending) noexcept;
        //�������ȷ������ʲô�ģ���ʱ��Ϊ����Ļ�ռ���
        Toning& postLightingBlending(BlendingMode blending) noexcept;
        //�������Ǹ��ռ��
        Toning& vertexDomain(VertexDomain domain) noexcept;
        Toning& culling(CullingMode culling) noexcept;
        Toning& colorWrite(spbool enable) noexcept;
        Toning& depthWrite(spbool enable) noexcept;
        Toning& depthCulling(spbool enable) noexcept;
        Toning& doubleSided(spbool doubleSided) noexcept;
        //���֣�alpha������ֵ�ᱻ�ü���
        Toning& maskThreshold(spfloat threshold) noexcept;
        //��Ӱ����
        Toning& shadowMultiplier(spbool shadowMultiplier) noexcept;
        //͸����Ӱ���ã��͹������Ǹ����
        Toning& transparentShadow(spbool transparentShadow) noexcept;
        //���û����͸��Ϳ�����������������������ʣ�IoR���仯
        Toning& clearCoatIorChange(spbool clearCoatIorChange) noexcept;
        Toning& flipUV(spbool flipUV) noexcept;
        Toning& multiBounceAmbientOcclusion(spbool multiBounceAO) noexcept;
        Toning& specularAmbientOcclusion(SpecularAmbientOcclusion specularAO) noexcept;
        Toning& refractionMode(RefractionMode refraction) noexcept;
        Toning& refractionType(RefractionType refractionType) noexcept;
        Toning& transparencyMode(TransparencyMode mode) noexcept;

        Result tone(SPStudioManager& manager, SPMaterial& material);

        //Toning& resources(size_t index, PrimitiveTopology type, spbool primitiveRestartEnable, SPBuffer* vertices, SPBuffer* indices/*, size_t offset, size_t minIndex, size_t maxIndex, size_t count*/) noexcept;
        //Result toning(SPStudioManager& manager, SPMaterial& material);
    private:
        std::vector<spchar> readShaderFile(const spstr& filename);
    public:

        struct Parameter {
            Parameter() noexcept : parameterType(INVALID) {}

            // Sampler
            Parameter(const spstr& paramName, SamplerType t, SamplerFormat f, Precision p)
                : name(paramName), size(1), precision(p), samplerType(t), format(f), parameterType(SAMPLER) { }

            // Uniform
            Parameter(const spstr& paramName, UniformType t, spint typeSize, Precision p)
                : name(paramName), size(typeSize), uniformType(t), precision(p), parameterType(UNIFORM) { }

            // Subpass
            Parameter(const spstr& paramName, SubpassType t, SamplerFormat f, Precision p)
                : name(paramName), size(1), precision(p), subpassType(t), format(f), parameterType(SUBPASS) { }

            spstr name;
            spint size;
            UniformType uniformType;
            Precision precision;
            SamplerType samplerType;
            SubpassType subpassType;
            SamplerFormat format;
            enum {
                INVALID,
                UNIFORM,
                SAMPLER,
                SUBPASS
            } parameterType;

            bool isSampler() const { return parameterType == SAMPLER; }
            bool isUniform() const { return parameterType == UNIFORM; }
            bool isSubpass() const { return parameterType == SUBPASS; }
        };

        static constexpr spint MAX_PARAMETERS_COUNT = 48;
        static constexpr spint MAX_SUBPASS_COUNT = 1;
        static constexpr spint MAX_ATTRIBUTES_COUNT = 7;
        //skill ��ʵ����shader
        struct Skill {
            spstr mId;
            spstr mMaterialName;
            spstr mFileName;
            Shading mShading = Shading::SP_LIT;
            Interpolation mInterpolation = Interpolation::SP_SMOOTH;
            Parameter mParameters[MAX_PARAMETERS_COUNT];
            spint mParameterCount = 0;
            spstr mVariables[MATERIAL_VARIABLES_COUNT];
            spuint mRequiredAttributes[MAX_ATTRIBUTES_COUNT];
            MaterialDomain mMaterialDomain = MaterialDomain::SP_SURFACE;
            ShaderQuality mShaderQuality = ShaderQuality::SP_DEFAULT;
            BlendingMode mBlendingMode = BlendingMode::SP_OPAQUE;
            BlendingMode mPostLightingBlendingMode = BlendingMode::SP_TRANSPARENT;
            VertexDomain mVertexDomain = VertexDomain::SP_OBJECT;
            CullingMode mCullingMode = CullingMode::SP_BACK;
            spbool mColorWrite = True;
            spbool mDepthWrite = True;
            //����
            spbool mDepthWriteSet = False;
            spbool mDepthTest = True;
            spbool mDoubleSided = False;
            //����
            spbool mDoubleSidedCapability = False;

            spfloat mMaskThreshold = 0.4f;
            spbool mShadowMultiplier = False;
            spbool mTransparentShadow = False;
            spbool mClearCoatIorChange = True;
            spbool mFlipUV = True;
            spbool mMultiBounceAO = False;
            spbool mMultiBounceAOSet = False;
            SpecularAmbientOcclusion mSpecularAO = SpecularAmbientOcclusion::SP_NONE;
            spbool mSpecularAOSet = False;
            TransparencyMode mTransparencyMode = TransparencyMode::SP_DEFAULT;
            RefractionMode mRefractionMode = RefractionMode::SP_NONE;
            RefractionType mRefractionType = RefractionType::SP_SOLID;

            spstr mVertexShaderPath;
            spstr mFragmentShaderPath;
        };
    };


protected:
	void addMaterial(SPMaterial* material, SPNeMu* nemu);
protected:
	
private:
	SPMangaka* boss;
	SPStudioManager* manager;
};

#endif
