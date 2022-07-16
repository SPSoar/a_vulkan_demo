#ifndef S_SPCOMPONENTTOOL_H_
#define S_SPCOMPONENTTOOL_H_

#include "SPSCamera.h"
#include "SPSImage.h"
#include "SPSLight.h"
#include "SPSMesh.h"
#include "SPSSampler.h"
#include "SPSTexture.h"


#include "SPSNode.h"
#include "SPSPBRMaterial.h"
#include "SPSScene.h"
#include "SPSSubMesh.h"
#include "SPSAnimationSampler.h"


class SPComponentTool
{
public:
	//static std::unique_ptr<SPSSubMesh> load_model(const std::string& file, uint32_t index = 0);
	static SPSLight& add_directional_light(SPSScene& scene, const glm::quat& rotation, const SPSLightProperties& props = {}, SPSNode* parent_node = nullptr);
	static SPSLight& add_light(SPSScene& scene, SPSLightType type, const glm::vec3& position, const glm::quat& rotation = {}, const SPSLightProperties& props = {}, SPSNode* parent_node = nullptr);

};

#endif