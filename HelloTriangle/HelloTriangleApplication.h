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

	int													Run( void );
private:
	void												InitVulkan( void );
	void												InitWindow( void );

	std::unique_ptr<std::vector<VkExtensionProperties>>	GetAvailableExtensions();

	void												CreateInstance( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions );
	bool												IsExtensionAvailable( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions, const char* extensionName );
	void												PrintExtensions( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions );

	void												MainLoop( void );

	std::unique_ptr<VKWrapper<VkInstance>>				m_vulkanInstance{ nullptr };
	GLFWwindow*											m_window{ nullptr };

	const int											WIDTH { 800 };
	const int											HEIGHT{ 600 };
};

}

#endif // !__HELLOTRIANGLEAPPLICATION_H__

