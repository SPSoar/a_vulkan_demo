#ifndef S_SPMATERIAL_H_
#define S_SPMATERIAL_H_
#include "../SPBase.h"
#include "SPTextureSampler.h"

enum ShaderType {
	Vertex,
	Fragment
};

class SPColoringAssistant;
class  SPMaterial : public SPBase
{
	friend class SPColoringAssistant;
public:
	SPMaterial() = default;
	virtual ~SPMaterial() = default;
	virtual void addTextureSampler(SPTextureSampler& textureSampler);
protected:
	std::unordered_map<spint, SPTextureSampler> textures;
	SPBuffer* vertexShaderBuffer;
	SPBuffer* fragmentShaderBuffer;
};

#endif
