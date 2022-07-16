#include "SPMangaStudio.h"
#include "SPMangaka.h"
SINGLETON_CREATE(SPMangaStudio);

void SPMangaStudio::create(Backend backend, Platform platform)
{
	this->s_manager = SPDriverFactory::CreateDriver(backend, platform);
	this->s_manager->initDriver();
}

void SPMangaStudio::invitePublisher(void* nativeWindow)
{
	this->s_manager->createSurface(nativeWindow);
}

SPStudioManager* SPMangaStudio::getManager()
{
	return this->s_manager;
}

SPMangaka* SPMangaStudio::inviteMangaka()
{
	return SPMangaka::get()->authorize(s_manager);
}

SPDrawingPad* SPMangaStudio::provideDrawingPad()
{
	return this->s_manager->createDrawingBoard();
}

void SPMangaStudio::rensai(SPMangaka* mangaka)
{
	mangaka->creatGenko();
}

void SPMangaStudio::createDrawingBoard()
{
	this->s_manager->createDrawingBoard();
}

