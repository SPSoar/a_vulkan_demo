#include "SPNeMu.h"
SPNeMu::SPNeMu()
{
}

SPNeMu::~SPNeMu()
{
}


void SPNeMu::submitSketch(SPSketch* sketch)
{
}


void SPNeMu::addCamera(SPCamera* camera)
{
	if (camera)
		cameras[camera->id] = camera;
}

void SPNeMu::addLight(SPLight* light)
{
	if (light)
		lights[light->id] = light;
}

void SPNeMu::addSky(SPSky* sky)
{
	if (sky)
		skies[sky->id] = sky;
}
