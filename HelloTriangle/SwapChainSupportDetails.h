#ifndef __SWAPCHAINSUPPORTDETAILS_H__
#define __SWAPCHAINSUPPORTDETAILS_H__

#include <vulkan\vulkan.h>
#include <vector>

namespace tut {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR		capabilities;
		std::vector<VkSurfaceFormatKHR>	formats;
		std::vector<VkPresentModeKHR>	presentModes;
	};
}

#endif