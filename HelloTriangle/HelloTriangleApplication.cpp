#include "HelloTriangleApplication.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

namespace tut {
/*
===============
HelloTriangle::run

	Runs the triangle application
===============
*/
int HelloTriangleApplication::run( void ) {
	try {
		initVulkan();
		initWindow();
		mainLoop();
	} catch ( const std::runtime_error& err ) {
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
/*
===============
HelloTriangle::initVulkan

	Initializes the Vulkan environment
===============
*/
void HelloTriangleApplication::initVulkan( void ) {

}
/*
===============
HelloTriangle::initWindow

	Creates a window as a render target
===============
*/
void HelloTriangleApplication::initWindow( void ) {
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API ); //Don't use OpenGL from now on
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	m_window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
}
/*
===============
HelloTriangle::mainLoop

	The main run loop for the application
===============
*/
void HelloTriangleApplication::mainLoop( void ) {
	while ( !glfwWindowShouldClose( m_window ) ) {
		glfwPollEvents();
	}
}
}