#include "SPMangaka.h"
#include "SPNeMu.h"
SINGLETON_CREATE(SPMangaka);


SPColoringAssistant* SPMangaka::employColoringAssistant()
{
	if (coloringAssistant) 
		return coloringAssistant;
	return coloringAssistant = new SPColoringAssistant(this);
}

SPInkingAssistant* SPMangaka::employInkingAssistant()
{
	if (inkingAssistant)
		return inkingAssistant;
	return inkingAssistant = new SPInkingAssistant(this);
}

SPPostProcessingAssistant* SPMangaka::employPostProcessingAssistant()
{
	if (postProcessingAssistant)
		return postProcessingAssistant;
	return postProcessingAssistant = new SPPostProcessingAssistant(this);
}

void SPMangaka::useDrawingPad(SPDrawingPad* drawingPad, const Canvas& canvas)
{
	this->drawingPad = drawingPad;
	this->canvas = canvas;
	drawingPad->powerUp(this->s_manager);
	drawingPad->cleanUpDrawingPad(this->canvas);
}

SPWorkFlow* SPMangaka::selectWorkflow(WorkFlowType wfType)
{
	if (workFlow)
		return workFlow;
	return workFlow =  s_manager->createWorkFlow(wfType);
}

SPSky* SPMangaka::createSky()
{
	return nullptr;
}

SPLight* SPMangaka::createLight()
{
	return nullptr;
}

SPCamera* SPMangaka::createCamera()
{
	return nullptr;
}

SPNeMu* SPMangaka::createNeMu()
{
	if (nemu)
		return nemu;
	return nemu = new SPNeMu();
}

SPSketch* SPMangaka::createSketch()
{
	return nullptr;
}

void SPMangaka::creatGenko()
{
	this->drawingPad->creatGenko(workFlow);
}

void SPMangaka::assignInkingTask(SPModelConfig& slConfig)
{
	inkingAssistant->inkDraft(slConfig);
}

void SPMangaka::assignColoringTask(SPDraft* model)
{
	coloringAssistant->coloring(model, nemu);
}


void SPMangaka::purchaseMaterial(SPMaterialConfig& matConfig)
{
	coloringAssistant->purchaseMaterial(matConfig, nemu);
}

void SPMangaka::changeMaterialForOneDraft(const spstr& draftId, const spstr& materialId)
{
	coloringAssistant->coloringDraftByOneMaterial(materialId, draftId, nemu);
}

void SPMangaka::destoryDraft(const spstr& id)
{
	inkingAssistant->destoryDraft(id, nemu);
}

void SPMangaka::destoryAllDrafts()
{
	inkingAssistant->destoryAllDrafts(nemu);
}

void SPMangaka::destoryMaterial(const spstr& id)
{
	coloringAssistant->destoryMaterial(id, nemu);
}

void SPMangaka::destoryAllMaterials()
{
	coloringAssistant->destoryAllMaterials(nemu);
}

void SPMangaka::destoryTexture(const spstr& id)
{
	coloringAssistant->destoryTexture(id, nemu);
}

void SPMangaka::destoryAllTexture()
{
	coloringAssistant->destoryAllTexture(nemu);
}

void SPMangaka::receiveDraft(SPDraft* model)
{
	if (model) 
	{
		nemu->drafts[model->id] = model;
		assignColoringTask(model);
	}
		
}

SPMangaka* SPMangaka::authorize(SPStudioManager* driver)
{
	this->s_manager = driver;
	return this;
}
