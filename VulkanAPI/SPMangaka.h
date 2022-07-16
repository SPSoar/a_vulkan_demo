#ifndef S_SPMANGAKA_H_
#define S_SPMANGAKA_H_

#include "SPSingleton.h"
#include "studio_common.h"
class SPMangaStudio;
class SPNeMu;
class SPDraft;
class RHIEXPORT SPMangaka
{
	friend SPMangaStudio;
	friend SPInkingAssistant;
	friend SPColoringAssistant;
	friend SPPostProcessingAssistant;
public:
	SPColoringAssistant* employColoringAssistant();
	SPInkingAssistant* employInkingAssistant();
	SPPostProcessingAssistant* employPostProcessingAssistant();
	void useDrawingPad(SPDrawingPad* drawingPad, const Canvas& canvas);
	SPWorkFlow* selectWorkflow(WorkFlowType wfType);
	SPSky* createSky();
	SPLight* createLight();
	SPCamera* createCamera();

	SPNeMu* createNeMu();
	SPSketch* createSketch();
	void creatGenko();

	void assignInkingTask(SPModelConfig& slConfig);
	void assignColoringTask(SPDraft* model);

	void purchaseMaterial(SPMaterialConfig& matConfig);

	void changeMaterialForOneDraft(const spstr& draftId, const spstr& materialId);

	void destoryDraft(const spstr& id);
	void destoryAllDrafts();
	void destoryMaterial(const spstr& id);
	void destoryAllMaterials();
	void destoryTexture(const spstr& id);
	void destoryAllTexture();
protected:
	void receiveDraft(SPDraft* model);
protected:
	SINGLETON_PUBLIC(SPMangaka);
	SPMangaka* authorize(SPStudioManager* driver);
	SINGLETON_PROTECTED(SPMangaka);
protected:
	SPStudioManager* s_manager = nullptr;
	SPDrawingPad* drawingPad = nullptr;
	SPInkingAssistant* inkingAssistant = nullptr;
	SPColoringAssistant* coloringAssistant = nullptr;
	SPPostProcessingAssistant* postProcessingAssistant = nullptr;
	SPNeMu* nemu = nullptr;
	SPWorkFlow* workFlow = nullptr;
private:
	Canvas canvas;
};

#endif
