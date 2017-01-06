#ifndef __HELLOTRIANGLEAPPLICATION_H__
#define __HELLOTRIANGLEAPPLICATION_H__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include "VKWrapper.h"
#include "QueueFamilyIndicies.h"
#include "SwapChainSupportDetails.h"

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
	bool													CheckDeviceExtensionSupport( VkPhysicalDevice device );
	QueueFamilyIndicies										FindQueueFamilies( VkPhysicalDevice device );

	void													CreateLogicalDevice( void );

	void													CreateSurface( void );
	
	SwapChainSupportDetails									QuerySwapChainSupport( VkPhysicalDevice device );
	VkSurfaceFormatKHR										ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats );
	VkPresentModeKHR										ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& presentModes );
	VkExtent2D												ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilites );
	void													CreateSwapChain( void );

	void													CreateImageViews( void );

	void													CreateGraphicsPipeline( void );
	std::vector<char>										ReadFile( const std::string& filePath );
	void													CreateShaderModule( const std::vector<char>& code, VKWrapper<VkShaderModule>& shaderModule );

	std::unique_ptr<VKWrapper<VkInstance>>					m_vulkanInstance{ nullptr };
	std::unique_ptr<VKWrapper<VkDevice>>					m_vulkanDevice{ nullptr };
	std::unique_ptr<VKWrapper<VkDebugReportCallbackEXT>>	m_vulkanDebugCallback{ nullptr };
	std::unique_ptr<VKWrapper<VkSurfaceKHR>>				m_windowSurface{ nullptr };
	std::unique_ptr<VKWrapper<VkSwapchainKHR>>				m_swapchain{ nullptr };

	std::vector<VkImage>									m_swapChainImages;
	std::vector<std::unique_ptr<VKWrapper<VkImageView>>>	m_swapChainImageViews;

	VkFormat												m_swapChainImageFormat;
	VkExtent2D												m_swapChainExtent;
	VkPhysicalDevice										m_selectedPhysicalDevice{ VK_NULL_HANDLE };
	VkQueue													m_presentQueue{ VK_NULL_HANDLE };

	GLFWwindow*												m_window{ nullptr };

	const uint32_t											WIDTH { 800 };
	const uint32_t											HEIGHT{ 600 };
	const std::vector<const char*>							VALIDATION_LAYERS{ "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*>							DEVICE_EXTENSIONS{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
	const bool												ENABLE_VALIDATION_LAYERS{ false };
#else
	const bool												ENABLE_VALIDATION_LAYERS{ true };
#endif // DEBUG
};

}

#endif // !__HELLOTRIANGLEAPPLICATION_H__

