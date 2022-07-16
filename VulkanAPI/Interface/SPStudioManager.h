#ifndef S_SPDRIVER_H_
#define S_SPDRIVER_H_
#include "common.h"
#include "../studio_enum.h"
class SPDrawingPad;
class SPSky;
class SPLight;
class SPCamera;
class SPDraft;
class SPMaterial;
class SPBuffer;
class SPWorkFlow;
class SPTextureSampler;
class SPStudioManager
{
public:
	virtual void initDriver() = 0;
	virtual void createSurface(void* nativeWindow) = 0;
	virtual SPBuffer* createIndexBuffer(std::vector<spint>& indices) = 0;
	virtual SPBuffer* createVertexBuffer(std::vector<SPVertex>& vertices) = 0;
	virtual void createTextureSampler(SPTextureSampler& textureSampler) = 0;
	virtual SPBuffer* createShaderModule(const std::vector<spchar>& code) = 0;
	

	virtual void destroySurface() = 0;
	virtual void destroyDriver() = 0;

	virtual void destroyVertexBuffer(SPBuffer* vBuffer) = 0;
	virtual void destroyIndexBufferr(SPBuffer* iBuffer) = 0;
	virtual void destroyShaderModule(SPBuffer* sBuffer) = 0;
	virtual void destroyTextureSampler(SPTextureSampler& textureSampler) = 0;
	//
	virtual SPDrawingPad* createDrawingBoard() = 0;
	virtual SPDraft* createDraft() = 0;
	virtual SPMaterial* createMaterial() = 0;
	virtual SPWorkFlow* createWorkFlow(WorkFlowType wfType) = 0;
};

#endif