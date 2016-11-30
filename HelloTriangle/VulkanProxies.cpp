#include "VulkanProxies.h"

namespace tut {
/*
===============
VulkanProxies::SetupDebugReportCallbackEXT

	Proxy for the vkCreateDebugReportCallbackEXT function
===============
*/
VkResult VulkanProxies::CreateDebugReportCallbackEXT( 
	VkInstance instance,
	const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugReportCallbackEXT* pCallback 
) {
	PFN_vkCreateDebugReportCallbackEXT func = ( PFN_vkCreateDebugReportCallbackEXT )vkGetInstanceProcAddr( instance, "vkCreateDebugReportCallbackEXT" );
	if ( func != nullptr ) {
		return func( instance, pCreateInfo, pAllocator, pCallback );
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}
/*
===============
VulkanProxies::DestroyDebugReportCallbackEXT

	Proxy for the vkCreateDebugReportCallbackEXT function
===============
*/
void VulkanProxies::DestroyDebugReportCallbackEXT( 
	VkInstance instance,
	VkDebugReportCallbackEXT callback,
	const VkAllocationCallbacks* pAllocator
) {
	PFN_vkDestroyDebugReportCallbackEXT func = ( PFN_vkDestroyDebugReportCallbackEXT )vkGetInstanceProcAddr( instance, "vkDestroyDebugReportCallbackEXT" );
	if ( func != nullptr ) {
		func( instance, callback, pAllocator );
	}
}
}