#include <vulkan\vulkan.h>
#include <memory>

#include "HelloTriangleApplication.h"

int main( int argc, char** argv ) {
	std::unique_ptr<tut::HelloTriangleApplication> application = std::make_unique<tut::HelloTriangleApplication>();

	return application->Run();
}