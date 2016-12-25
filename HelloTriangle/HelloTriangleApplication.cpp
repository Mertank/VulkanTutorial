#include "HelloTriangleApplication.h"
#include "VulkanProxies.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>

namespace tut {
/*
===============
HelloTriangleApplication::DebugCallback

	Debug message callback from the Vulkan validation layers
===============
*/
VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData
) {
	std::cerr << msg << std::endl;

	return VK_FALSE;
}
/*
===============
HelloTriangleApplication::HelloTriangleApplication

	HelloTriangleApplication default constructor
===============
*/
HelloTriangleApplication::HelloTriangleApplication( void ) 
{}
/*
===============
HelloTriangleApplication::Run

	Runs the triangle application
===============
*/
int HelloTriangleApplication::Run( void ) {
	try {
		InitWindow();
		InitVulkan();
		MainLoop();
	} catch ( const std::runtime_error& err ) {
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
/*
===============
HelloTriangleApplication::InitVulkan

	Initializes the Vulkan environment
===============
*/
void HelloTriangleApplication::InitVulkan( void ) {
	if ( ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport() ) {
		std::cerr << "Validation layers are not available" << std::endl;
		throw std::runtime_error( "Validation layers were requested, but not available!" );
	}

	CreateInstance();
	SetupDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
}
/*
===============
HelloTriangleApplication::SetupDebugCallback

	Hooks the debug callback into the vulkan debug layer
===============
*/
void HelloTriangleApplication::SetupDebugCallback( void ) {
	//Don't do this if the validation layers are disabled
	if ( !ENABLE_VALIDATION_LAYERS ) {
		return;
	}

	VkDebugReportCallbackCreateInfoEXT createInfo = {};

	createInfo.sType		= VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	createInfo.flags		= VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT; //Enable warnings and errors
	createInfo.pfnCallback	= DebugCallback;

	//Init unique pointer and use feed it to Vulkan
	m_vulkanDebugCallback = std::make_unique<VKWrapper<VkDebugReportCallbackEXT>>( *m_vulkanInstance, VulkanProxies::DestroyDebugReportCallbackEXT );
	if ( VulkanProxies::CreateDebugReportCallbackEXT( *m_vulkanInstance, &createInfo, nullptr, m_vulkanDebugCallback->replace() ) != VK_SUCCESS ) {
		throw std::runtime_error( "Failed to setup debug callback" );
	}
}
/*
===============
HelloTriangleApplication::GetAvailableExtensions

	Returns if the extension was found in a list of extensions
===============
*/
std::unique_ptr<std::vector<VkExtensionProperties>> HelloTriangleApplication::GetAvailableExtensions( void ) {
	//Query driver for extension count
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

	//Resd the extensions from the driver into the vector
	std::unique_ptr<std::vector<VkExtensionProperties>> availableExtensions = std::make_unique<std::vector<VkExtensionProperties>>( extensionCount );
	vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, availableExtensions->data() );

	return availableExtensions;
}
/*
===============
HelloTriangleApplication::IsExtensionAvailable

	Returns if the extension name was found in the available extensions
===============
*/
bool HelloTriangleApplication::IsExtensionAvailable( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions, const char* extensionName ) {
	for ( const VkExtensionProperties& extension : *availableExtensions ) {
		if ( strcmp( extension.extensionName, extensionName ) == 0 ) {
			return true;
		}
	}

	return false;
}
/*
===============
HelloTriangleApplication::CreateInstance

	Creates a Vulkan instance
===============
*/
void HelloTriangleApplication::CreateInstance( void ) {
	VkApplicationInfo							applicationInfo		= {};
	VkInstanceCreateInfo						instanceInfo		= {};
	std::unique_ptr<std::vector<const char*>>	requiredExtensions	= GetRequiredExtensions();

	//Setup application info
	applicationInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName	= "Hello Triangle";
	applicationInfo.applicationVersion	= VK_MAKE_VERSION( 1, 0, 0 );
	applicationInfo.pEngineName			= "No Engine";
	applicationInfo.engineVersion		= VK_MAKE_VERSION( 1, 0, 0 );
	applicationInfo.apiVersion			= VK_API_VERSION_1_0;

	//Setup instance creation information
	instanceInfo.sType				= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo	= &applicationInfo;

	//Pull extension information from GLFW

	instanceInfo.enabledExtensionCount		= requiredExtensions->size();
	instanceInfo.ppEnabledExtensionNames	= requiredExtensions->data();
	
	if ( ENABLE_VALIDATION_LAYERS ) {
		instanceInfo.enabledLayerCount		= VALIDATION_LAYERS.size();
		instanceInfo.ppEnabledLayerNames	= VALIDATION_LAYERS.data();
	} else {
		instanceInfo.enabledLayerCount = 0;
	}

	//Initialize the wrapper for our instance
	m_vulkanInstance = std::make_unique<VKWrapper<VkInstance>>( vkDestroyInstance );

	//Create the instance and store it in the wrapper
	if ( vkCreateInstance( &instanceInfo, nullptr, m_vulkanInstance->replace() ) != VK_SUCCESS ) {
		throw std::runtime_error( "Could not create a Vulkan Instance!" );
	}
}
/*
===============
HelloTriangleApplication::CheckValidationLayerSupport

	Checks if the required validation layers are available
===============
*/
bool HelloTriangleApplication::CheckValidationLayerSupport( void ) {
	uint32_t	layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> layerProperties( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, layerProperties.data() );

	for ( const char* layerName : VALIDATION_LAYERS ) {
		bool layerFound = false;

		for ( const VkLayerProperties& properties : layerProperties ) {
			if ( strcmp( properties.layerName, layerName ) == 0 ) {
				layerFound = true;
				break;
			}
		}

		if ( !layerFound ) {
			return false;
		}
	}

	return true;
}
/*
===============
HelloTriangleApplication::CheckExtensionSupport

	Makes sure all the requred extensions are available
===============
*/
bool HelloTriangleApplication::CheckExtensionSupport( const std::unique_ptr<std::vector<const char*>>& requiredExtensions ) {
	std::unique_ptr<std::vector<VkExtensionProperties>> availableExtensions = GetAvailableExtensions();

	for ( const char* ext : *requiredExtensions ) {
		bool foundExtension = false;

		for ( const VkExtensionProperties& extensionProperties : *availableExtensions ) {
			if ( strcmp( extensionProperties.extensionName, ext ) == 0 ) {
				foundExtension = true;
				break;
			}
		}

		if ( !foundExtension ) {
			return false;
		}
	}

	return true;
}
/*
===============
HelloTriangleApplication::GetRequiredExtensions

	Returns the extensions this application requires to run
===============
*/
std::unique_ptr<std::vector<const char*>> HelloTriangleApplication::GetRequiredExtensions( void ) {
	std::unique_ptr<std::vector<const char*>>	extensions		= std::make_unique<std::vector<const char*>>();
	uint32_t									extensionCount	= 0;
	const char**								glfwExtensions	= glfwGetRequiredInstanceExtensions( &extensionCount );

	for ( uint32_t i = 0; i < extensionCount; ++i ) {
		extensions->push_back( glfwExtensions[ i ] );
	}

	if ( ENABLE_VALIDATION_LAYERS ) {
		extensions->push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
	}

	if ( !CheckExtensionSupport( extensions ) ) {
		std::cerr << "Extensions are unavailable!" << std::endl;
		throw std::runtime_error( "Extensions that were required are not available!" );
	}

	return extensions;
}
/*
===============
HelloTriangleApplication::InitWindow

	Creates a window as a render target
===============
*/
void HelloTriangleApplication::InitWindow( void ) {
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API ); //Don't use OpenGL from now on
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	m_window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
}
/*
===============
HelloTriangleApplication::CreateSurface

	Creates the window surface for rendering
===============
*/
void HelloTriangleApplication::CreateSurface( void ) {
	m_windowSurface = std::make_unique<VKWrapper<VkSurfaceKHR>>( *m_vulkanInstance, vkDestroySurfaceKHR );

	if ( glfwCreateWindowSurface( *m_vulkanInstance, m_window, nullptr, m_windowSurface->replace() ) != VK_SUCCESS ) {
		throw std::runtime_error( "Failed to create window surface for rendering" );
	}
}
/*
===============
HelloTriangleApplication::PickPhysicalDevice

	Picks the physical device to use for rendering
===============
*/
void HelloTriangleApplication::PickPhysicalDevice( void ) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices( *m_vulkanInstance, &deviceCount, nullptr );

	if ( deviceCount == 0 ) {
		throw std::runtime_error( "No GPU that supports Vulkan was found" );
	}

	std::vector<VkPhysicalDevice> devices( deviceCount );
	vkEnumeratePhysicalDevices( *m_vulkanInstance, &deviceCount, devices.data() );

	for ( const VkPhysicalDevice& device : devices ) {
		if ( IsDeviceSuitable( device ) ) {
			m_selectedPhysicalDevice = device;
			break;
		}
	}

	if ( m_selectedPhysicalDevice == VK_NULL_HANDLE ) {
		throw std::runtime_error( "No GPU found that's suitable" );
	}
}
/*
===============
HelloTriangleApplication::IsDeviceSuitable

	Returns if the device is suitable for what functions we need.
===============
*/
bool HelloTriangleApplication::IsDeviceSuitable( VkPhysicalDevice device ) {
	QueueFamilyIndicies indicies			= FindQueueFamilies( device );
	bool				extensionsAvailable = CheckDeviceExtensionSupport( device );
	bool				swapChainAdequate	= false;

	if ( extensionsAvailable ) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( device );
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indicies.IsComplete() && extensionsAvailable && swapChainAdequate;
}
/*
===============
HelloTriangleApplication::CheckDeviceExtensionSupport

	Checks if all the required extensions are available
===============
*/
bool HelloTriangleApplication::CheckDeviceExtensionSupport( VkPhysicalDevice device ) {
	uint32_t deviceExtensionCount;
	vkEnumerateDeviceExtensionProperties( device, nullptr, &deviceExtensionCount, nullptr );

	std::vector<VkExtensionProperties> availableExtensions( deviceExtensionCount );
	vkEnumerateDeviceExtensionProperties( device, nullptr, &deviceExtensionCount, availableExtensions.data() );

	std::set<std::string> requiredExtensions( DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end() );

	for ( const VkExtensionProperties& extension : availableExtensions ) {
		requiredExtensions.erase( extension.extensionName );
	}

	return requiredExtensions.empty();
}
/*
===============
HelloTriangleApplication::FindQueueFamilies

	Checks if the device supports graphics queue families
===============
*/
QueueFamilyIndicies HelloTriangleApplication::FindQueueFamilies( VkPhysicalDevice device ) {
	QueueFamilyIndicies indicies;
	uint32_t			queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

	int index = 0;
	for ( const VkQueueFamilyProperties& queueFamily : queueFamilies ) {
		if ( queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
			indicies.GraphicsFamily = index;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR( device, index, *m_windowSurface, &presentSupport );
		
		if ( queueFamily.queueCount > 0 && presentSupport ) {
			indicies.PresentFamily = index;
		}

		if ( indicies.IsComplete() ) {
			break;
		}

		++index;
	}

	return indicies;
}
/*
===============
HelloTriangleApplication::CreateLogicalDevice

	Creates logical device for rendering
===============
*/
void HelloTriangleApplication::CreateLogicalDevice( void ) {
	QueueFamilyIndicies						indicies			= FindQueueFamilies( m_selectedPhysicalDevice );

	std::vector<VkDeviceQueueCreateInfo>	queueCreateInfos;
	std::set<int>							uniqueQueueFamilies = { indicies.GraphicsFamily, indicies.PresentFamily };
	
	VkPhysicalDeviceFeatures				deviceFeatures		= {};
	VkDeviceCreateInfo						deviceCreateInfo	= {};

	float									queuePriority		= 1.0f;

	for ( int queueFamily : uniqueQueueFamilies ) {
		VkDeviceQueueCreateInfo	queueCreateInfo = {};

		queueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex	= queueFamily;
		queueCreateInfo.queueCount			= 1;
		queueCreateInfo.pQueuePriorities	= &queuePriority;

		queueCreateInfos.push_back( queueCreateInfo );
	}
	
	deviceCreateInfo.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos		= queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount	= ( uint32_t )queueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures		= &deviceFeatures;
	
	deviceCreateInfo.enabledExtensionCount		= DEVICE_EXTENSIONS.size();
	deviceCreateInfo.ppEnabledExtensionNames	= DEVICE_EXTENSIONS.data();

	if ( ENABLE_VALIDATION_LAYERS ) {
		deviceCreateInfo.enabledLayerCount		= VALIDATION_LAYERS.size();
		deviceCreateInfo.ppEnabledLayerNames	= VALIDATION_LAYERS.data();
	} else {
		deviceCreateInfo.enabledLayerCount = 0;
	}

	m_vulkanDevice = std::make_unique<VKWrapper<VkDevice>>( vkDestroyDevice );
	if ( vkCreateDevice( m_selectedPhysicalDevice, &deviceCreateInfo, nullptr, m_vulkanDevice->replace() ) != VK_SUCCESS ) {
		throw std::runtime_error( "Failed to create logical device" );
	}

	vkGetDeviceQueue( *m_vulkanDevice, indicies.PresentFamily, 0, &m_presentQueue );
}
/*
===============
HelloTriangleApplication::QuerySwapChainSupport

	Queries information about the swapchain support
===============
*/
SwapChainSupportDetails HelloTriangleApplication::QuerySwapChainSupport( VkPhysicalDevice device ) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, *m_windowSurface, &details.capabilities );

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR( device, *m_windowSurface, &formatCount, nullptr );

	if ( formatCount != 0 ) {
		details.formats.resize( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, *m_windowSurface, &formatCount, details.formats.data() );
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR( device, *m_windowSurface, &presentModeCount, nullptr );

	if ( presentModeCount != 0 ) {
		details.presentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, *m_windowSurface, &presentModeCount, details.presentModes.data() );
	}

	return details;
}
/*
===============
HelloTriangleApplication::ChooseSwapSurfaceFormat

	Select the best available format
===============
*/
VkSurfaceFormatKHR HelloTriangleApplication::ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& formats ) {
	if ( formats.size() == 1 && formats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	for ( const VkSurfaceFormatKHR& format : formats ) {
		if ( format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_R8G8B8A8_UNORM ) {
			return format;
		}
	}

	return formats[ 0 ];
}
/*
===============
HelloTriangleApplication::ChooseSwapPresentMode

	Select the best present mode that's available
===============
*/
VkPresentModeKHR HelloTriangleApplication::ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& presentModes ) {
	for ( const VkPresentModeKHR& presentMode : presentModes ) {
		if ( presentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
			return presentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR; //Guaranteed to exist
}
/*
===============
HelloTriangleApplication::ChooseSwapExtent

	Choose swap chain resolution
===============
*/
VkExtent2D HelloTriangleApplication::ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities ) {
	if ( capabilities.currentExtent.width != std::numeric_limits<uint32_t>().max() ) {
		return capabilities.currentExtent;
	} else {
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width	= std::max( capabilities.minImageExtent.width , std::min( capabilities.maxImageExtent.width , actualExtent.width  ) );
		actualExtent.height = std::max( capabilities.minImageExtent.height, std::min( capabilities.maxImageExtent.height, actualExtent.height ) );

		return actualExtent;
	}
}
/*
===============
HelloTriangleApplication::CreateSwapChain

	Creates the swap chain
===============
*/
void HelloTriangleApplication::CreateSwapChain( void ) {
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( m_selectedPhysicalDevice );

	VkSurfaceFormatKHR		swapChainSurfaceFormat	= ChooseSwapSurfaceFormat( swapChainSupport.formats );
	VkPresentModeKHR		swapChainPresentMode	= ChooseSwapPresentMode( swapChainSupport.presentModes );
	VkExtent2D				swapChainExtents		= ChooseSwapExtent( swapChainSupport.capabilities );

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if ( swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount ) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};

	createInfo.sType	= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface	= *m_windowSurface;

	createInfo.minImageCount	= imageCount;
	createInfo.imageFormat		= swapChainSurfaceFormat.format;
	createInfo.imageColorSpace	= swapChainSurfaceFormat.colorSpace;
	createInfo.imageExtent		= swapChainExtents;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndicies indicies				= FindQueueFamilies( m_selectedPhysicalDevice );
	uint32_t			queueFamilyIndicies[]	= { ( uint32_t )indicies.GraphicsFamily, ( uint32_t )indicies.PresentFamily };

	if ( indicies.GraphicsFamily != indicies.PresentFamily ) {
		createInfo.imageSharingMode			= VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= 2;
		createInfo.pQueueFamilyIndices		= queueFamilyIndicies;
	} else {
		createInfo.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount	= 0;
		createInfo.pQueueFamilyIndices		= nullptr;
	}

	createInfo.preTransform		= swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode		= swapChainPresentMode;
	createInfo.clipped			= VK_TRUE;
	createInfo.oldSwapchain		= VK_NULL_HANDLE;

	m_swapchain = std::make_unique<VKWrapper<VkSwapchainKHR>>( *m_vulkanDevice, vkDestroySwapchainKHR );

	if ( vkCreateSwapchainKHR( *m_vulkanDevice, &createInfo, nullptr, m_swapchain->replace() ) != VK_SUCCESS ) {
		throw std::runtime_error( "Could not create swapchain" );
	}

	//Retrieve images
	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR( *m_vulkanDevice, *m_swapchain, &swapChainImageCount, nullptr );
	m_swapChainImages.resize( swapChainImageCount );
	vkGetSwapchainImagesKHR( *m_vulkanDevice, *m_swapchain, &swapChainImageCount, m_swapChainImages.data() );

	m_swapChainExtent		= swapChainExtents;
	m_swapChainImageFormat	= swapChainSurfaceFormat.format;
}
/*
===============
HelloTriangleApplication::mainLoop

	The main run loop for the application
===============
*/
void HelloTriangleApplication::MainLoop( void ) {
	while ( !glfwWindowShouldClose( m_window ) ) {
		glfwPollEvents();
	}
}
}