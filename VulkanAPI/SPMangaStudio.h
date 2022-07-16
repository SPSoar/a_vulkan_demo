#ifndef S_SPMANGASTUDIO_H_
#define S_SPMANGASTUDIO_H_
#include "SPSingleton.h"
#include "studio_common.h"

class SPMangaka;
class RHIEXPORT SPMangaStudio
{
public:
	SINGLETON_PUBLIC(SPMangaStudio);
	void create(Backend backend = Backend::VULKAN, Platform platform = Platform::QT_WINDOWS);
	void invitePublisher(void* nativeWindow);
	SPStudioManager* getManager();
	SPMangaka* inviteMangaka();
	SPDrawingPad* provideDrawingPad();
	void rensai(SPMangaka* mangaka);

	//以后移到外面
	void createDrawingBoard();
	//static void destroy(Engine** engine);
	//inking
	//coloring assistant
	//PostProcessing   //
	//EntityManager& getEntityManager() noexcept;
	//RenderableManager& getRenderableManager() noexcept;
	//LightManager& getLightManager() noexcept;
	//TransformManager& getTransformManager() noexcept;
	//SwapChain* createSwapChain(void* nativeWindow, uint64_t flags = 0) noexcept;
	//Renderer* createRenderer() noexcept;
	//View* createView() noexcept;
	//Scene* createScene() noexcept;
	//Camera* createCamera(utils::Entity entity) noexcept;
	//Fence* createFence() noexcept;

	//FBufferObject* createBufferObject(const BufferObject::Builder& builder) noexcept;
	//FVertexBuffer* createVertexBuffer(const VertexBuffer::Builder& builder) noexcept;
	//FIndexBuffer* createIndexBuffer(const IndexBuffer::Builder& builder) noexcept;
	//FSkinningBuffer* createSkinningBuffer(const SkinningBuffer::Builder& builder) noexcept;
	//FIndirectLight* createIndirectLight(const IndirectLight::Builder& builder) noexcept;
	//FMaterial* createMaterial(const Material::Builder& builder) noexcept;
	//FTexture* createTexture(const Texture::Builder& builder) noexcept;
	//FSkybox* createSkybox(const Skybox::Builder& builder) noexcept;
	//FColorGrading* createColorGrading(const ColorGrading::Builder& builder) noexcept;
	//FStream* createStream(const Stream::Builder& builder) noexcept;
	//FRenderTarget* createRenderTarget(const RenderTarget::Builder& builder) noexcept;


	//bool destroy(const BufferObject* p);        //!< Destroys a BufferObject object.
	//bool destroy(const VertexBuffer* p);        //!< Destroys an VertexBuffer object.
	//bool destroy(const Fence* p);               //!< Destroys a Fence object.
	//bool destroy(const IndexBuffer* p);         //!< Destroys an IndexBuffer object.
	//bool destroy(const IndirectLight* p);       //!< Destroys an IndirectLight object.

	//bool destroy(const Material* p);
	//bool destroy(const MaterialInstance* p);    //!< Destroys a MaterialInstance object.
	//bool destroy(const Renderer* p);            //!< Destroys a Renderer object.
	//bool destroy(const Scene* p);               //!< Destroys a Scene object.
	//bool destroy(const Skybox* p);              //!< Destroys a SkyBox object.
	//bool destroy(const ColorGrading* p);        //!< Destroys a ColorGrading object.
	//bool destroy(const SwapChain* p);           //!< Destroys a SwapChain object.
	//bool destroy(const Stream* p);              //!< Destroys a Stream object.
	//bool destroy(const Texture* p);             //!< Destroys a Texture object.
	//bool destroy(const RenderTarget* p);        //!< Destroys a RenderTarget object.
	//bool destroy(const View* p);                //!< Destroys a View object.
	//void destroy(utils::Entity e);              //!< Destroys all filament-known components from this entity

	//void flushAndWait();
	//const Material* getDefaultMaterial() const noexcept;
	//Backend getBackend() const noexcept;
	//Platform* getPlatform() const noexcept;
	//void execute();

	////----------------------------
	//backend::Driver& getDriver() const noexcept { return *mDriver; }

protected:
	SINGLETON_PROTECTED(SPMangaStudio);
protected:
private:
	SPStudioManager* s_manager;
};

#endif
