#include "HelloTriangleApplication.h"
#include "VulkanProxies.h"

#include <iostream>
#include <stdexcept>
#include <vector>

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