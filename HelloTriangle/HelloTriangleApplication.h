#ifndef __HELLOTRIANGLEAPPLICATION_H__
#define __HELLOTRIANGLEAPPLICATION_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include "VKWrapper.h"
#include "QueueFamilyIndicies.h"

namespace tut {

class HelloTriangleApplication {
public:
															HelloTriangleApplication( void );

	int														Run( void );
private:
	void													MainLoop( void );

	void													InitVulkan( void );
	void													InitWindow( void );

	std::unique_ptr<std::vector<VkExtensionProperties>>		GetAvailableExtensions( void );

	void													CreateInstance( void );
	bool													IsExtensionAvailable( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions, const char* extensionName );

	std::unique_ptr<std::vector<const char*>>				GetRequiredExtensions( void );

	bool													CheckValidationLayerSupport( void );
	bool													CheckExtensionSupport( const std::unique_ptr<std::vector<const char*>>& requiredExtensions );

	void													SetupDebugCallback( void );

	static VKAPI_ATTR VkBool32 VKAPI_CALL					DebugCallback(
																VkDebugReportFlagsEXT flags,
																VkDebugReportObjectTypeEXT objType,
																uint64_t obj,
																size_t location,
																int32_t code,
																const char* layerPrefix,
																const char* msg,
																void* userData
															);

	void													PickPhysicalDevice( void );
	bool													IsDeviceSuitable( VkPhysicalDevice device );
	QueueFamilyIndicies										FindQueueFamilies( VkPhysicalDevice device );

	void													CreateLogicalDevice( void );

	std::unique_ptr<VKWrapper<VkInstance>>					m_vulkanInstance{ nullptr };
	std::unique_ptr<VKWrapper<VkDevice>>					m_vulkanDevice{ nullptr };
	std::unique_ptr<VKWrapper<VkDebugReportCallbackEXT>>	m_vulkanDebugCallback{ nullptr };
	VkPhysicalDevice										m_selectedPhysicalDevice{ VK_NULL_HANDLE };

	GLFWwindow*												m_window{ nullptr };

	const int												WIDTH { 800 };
	const int												HEIGHT{ 600 };
	const std::vector<const char*>							VALIDATION_LAYERS{ "VK_LAYER_LUNARG_standard_validation" };

#ifdef NDEBUG
	const bool												ENABLE_VALIDATION_LAYERS{ false };
#else
	const bool												ENABLE_VALIDATION_LAYERS{ true };
#endif // DEBUG
};

}

#endif // !__HELLOTRIANGLEAPPLICATION_H__

