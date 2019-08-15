#pragma once

#include "expect.h"

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <functional>
#include <vector>

template <typename T = void>
void vkcall_0(VkResult expr, const char* msg, const char* file, int line) {
    if ((expr) != VK_SUCCESS) {
        printf("\033[0;31m%s:%d\033[0m %s\n", file, line, msg);
        throw std::runtime_error("");
    }
}

#define vkcall(expr) vkcall_0(expr, #expr, __FILE__, __LINE__)

#define VKF_INSTANCE(func) extern PFN_##func func;
#include "instance.inl"

#define VKF_DEVICE(func) extern PFN_##func func;
#include "device.inl"

struct TContext {
    VkInstance instance{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};
    uint32_t renderQueueFamilyIndex{0};
    VkQueue renderQueue{VK_NULL_HANDLE};
    uint32_t transferQueueFamilyIndex{0};
    VkQueue transferQueue{VK_NULL_HANDLE};
    uint32_t findMemoryIndex(uint32_t filter, VkMemoryPropertyFlags flags) {
        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);
        for (uint32_t i = 0; i < memory.memoryTypeCount; i++) {
            if ((1 << i) & filter) {
                if ((memory.memoryTypes[i].propertyFlags & flags) == flags) {
                    return i;
                }
            }
        }
        throw std::runtime_error("Could not find memory index");
    }
    VkFormat findFormat(std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR and (props.linearTilingFeatures & features) == features)
                return format;
            if (tiling == VK_IMAGE_TILING_OPTIMAL and (props.optimalTilingFeatures & features) == features)
                return format;
        }
        throw std::runtime_error("Could not find format");
    }
};

extern TContext ctx;

void createInstance(std::vector<const char*>& requiredExtensionNames);
void destroyInstance();

void createDevice(std::function<void(VkPhysicalDevice pdev, VkPhysicalDeviceFeatures& enabledFeatures)> validator);
void destroyDevice();
