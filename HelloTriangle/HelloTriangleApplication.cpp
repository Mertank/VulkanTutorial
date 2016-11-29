#include "HelloTriangleApplication.h"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace tut {
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
	std::unique_ptr<std::vector<VkExtensionProperties>> availableExtensions = GetAvailableExtensions();

	CreateInstance( availableExtensions );
	PrintExtensions( availableExtensions );
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
void HelloTriangleApplication::CreateInstance( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions ) {
	VkApplicationInfo		applicationInfo		= {};
	VkInstanceCreateInfo	instanceInfo		= {};
	uint32_t				glfwExtensionCount	= 0;
	const char**			glfwExtensions;

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
	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	instanceInfo.enabledExtensionCount		= glfwExtensionCount;
	instanceInfo.ppEnabledExtensionNames	= glfwExtensions;
	instanceInfo.enabledLayerCount			= 0;

	for ( uint32_t i = 0; i < glfwExtensionCount; ++i ) {
		if ( !IsExtensionAvailable( availableExtensions, glfwExtensions[ i ] ) ) {
			std::cerr << "Extension \'" << glfwExtensions[ i ] << "\' is not available!" << std::endl;
			throw std::runtime_error( "An extension for initializing Vulkan is missing!" );
		}
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
HelloTriangleApplication::PrintExtensions

	Cycles through all available extensions and prints them out
===============
*/
void HelloTriangleApplication::PrintExtensions( const std::unique_ptr<std::vector<VkExtensionProperties>>& availableExtensions ) {
	//Print out the extensions
	std::cout << "Available extensions:" << std::endl;

	for ( const VkExtensionProperties& extensionProperties : *availableExtensions ) {
		std::cout << "\t" << extensionProperties.extensionName << std::endl;
	}
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