#pragma once

#include <vulkan/vulkan.h>

class DynamicState
{
public:
	VkRect2D scissor;

	bool operator==(const DynamicState& rhs) const;
	bool operator!=(const DynamicState& rhs) const;
};
