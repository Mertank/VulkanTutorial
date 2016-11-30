#ifndef __VULKANPROXIES_H__
#define __VULKANPROXIES_H__

#include <vulkan\vulkan.h>

namespace tut {

class VulkanProxies {
public:
	static VkResult	CreateDebugReportCallbackEXT(
						VkInstance instance,
						const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
						const VkAllocationCallbacks* pAllocator,
						VkDebugReportCallbackEXT* pCallback
					);

	static void		DestroyDebugReportCallbackEXT(
						VkInstance instance,
						VkDebugReportCallbackEXT callback,
						const VkAllocationCallbacks* pAllocator
					);
};

}

#endif // !__VULKANPROXIES_H__

