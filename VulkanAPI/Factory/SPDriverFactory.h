#ifndef S_SPDRIVERFACTORY_H_
#define S_SPDRIVERFACTORY_H_
#include "../studio_enum.h"
#include "../Backend/VulkanDriver/SPVulkanWindowsDriver.h"
class SPDriverFactory
{
public:
	static SPStudioManager* CreateDriver(Backend backend, Platform platform) 
	{
		switch (backend)
		{
		case Backend::VULKAN:
			switch (platform)
			{
			case Platform::QT_WINDOWS:
				return SPVulkanWindowsDriver::get();
				break;
			case Platform::QT_MAC:
				break;
			case Platform::QT_ANDROID:
				break;
			case Platform::QT_IOS:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return nullptr;
	}
};

#endif