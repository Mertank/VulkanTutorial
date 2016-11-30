#ifndef __HELLOTRIANGLEAPPLICATION_H__
#define __HELLOTRIANGLEAPPLICATION_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include "VKWrapper.h"

namespace tut {

class HelloTriangleApplication {
public:
															HelloTriangleApplication( void );

	int														Run( void );
private:
	void													InitVulkan( void );
	void													InitWindow( void );

	std::unique_ptr<std::vector<VkExtensionProperties>>		GetAvailableExtensions();

	void													CreateInstance();
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

	void													MainLoop( void );

	std::unique_ptr<VKWrapper<VkInstance>>					m_vulkanInstance{ nullptr };
	std::unique_ptr<VKWrapper<VkDebugReportCallbackEXT>>	m_vulkanDebugCallback{ nullptr };
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

