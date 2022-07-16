#ifndef S_SPNEMU_H_
#define S_SPNEMU_H_
#include "studio_common.h"

class RHIEXPORT SPNeMu
{
	friend SPDrawingPad;
public:
	SPNeMu();
	virtual ~SPNeMu();

	void submitSketch(SPSketch* sketch);
	void addCamera(SPCamera* camera);
	void addLight(SPLight* light);
	void addSky(SPSky* sky);

public:

	std::unordered_map<spstr, SPSky*> skies;
	std::unordered_map<spstr, SPLight*> lights;
	std::unordered_map<spstr, SPCamera*> cameras;
	std::unordered_map<spstr, SPDraft*> drafts;
	std::unordered_map<spstr, SPMaterial*> materials;
	std::unordered_map<spstr, SPTextureSampler> textures;

};

#endif
