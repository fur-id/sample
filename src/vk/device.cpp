#include "vk_private.h"

#include <set>

void createDevice(std::function<void(VkPhysicalDevice pdev, VkPhysicalDeviceFeatures& enabledFeatures)> validator) {
    uint32_t count;
    vkcall(vkEnumeratePhysicalDevices(ctx.instance, &count, nullptr));
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkcall(vkEnumeratePhysicalDevices(ctx.instance, &count, physicalDevices.data()));
    struct SuitableDevice {
        VkPhysicalDevice physicalDevice;
        uint32_t renderQueueFamilyIndex;
        uint32_t transferQueueFamilyIndex;
        std::vector<const char*> enabledExtensionNames;
        VkPhysicalDeviceFeatures features{};
    };
    std::vector<SuitableDevice> suitableDevices;
    suitableDevices.reserve(count);
    for (VkPhysicalDevice pdev : physicalDevices) {
        SuitableDevice sd{};
        sd.physicalDevice = pdev;
        try {
            vkcall(vkEnumerateDeviceExtensionProperties(pdev, nullptr, &count, nullptr));
            std::vector<VkExtensionProperties> availableExtensions(count);
            vkcall(vkEnumerateDeviceExtensionProperties(pdev, nullptr, &count, availableExtensions.data()));
            std::vector<const char*> requiredExtensionNames{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            };
            for (const char* name : requiredExtensionNames) {
                bool found = false;
                for (VkExtensionProperties& props : availableExtensions) {
                    if (std::string(name) == props.extensionName) {
                        found = true;
                        sd.enabledExtensionNames.push_back(name);
                        break;
                    }
                }
                expect(found);
            }
            validator(pdev, sd.features);
            vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, nullptr);
            std::vector<VkQueueFamilyProperties> queueProps(count);
            vkGetPhysicalDeviceQueueFamilyProperties(pdev, &count, queueProps.data());
            for (uint32_t index = 0; index < count; index++) {
                if ((queueProps[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
                    continue;
                if ((queueProps[index].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
                    continue;
                VkBool32 p;
                vkcall(vkGetPhysicalDeviceSurfaceSupportKHR(pdev, index, ctx.surface, &p));
                if (!p)
                    continue;
                sd.renderQueueFamilyIndex = index;
                break;
            }
            sd.transferQueueFamilyIndex = sd.renderQueueFamilyIndex;
            for (uint32_t index = 0; index < count; index++) {
                if (sd.renderQueueFamilyIndex == index)
                    continue;
                if ((queueProps[index].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
                    continue;
                sd.transferQueueFamilyIndex = index;
                break;
            }
            suitableDevices.push_back(sd);
        } catch (...) {
        }
    }
    expect(!suitableDevices.empty());

    SuitableDevice& sd = suitableDevices[0];

    std::set<uint32_t> indices{
        sd.renderQueueFamilyIndex,
        sd.transferQueueFamilyIndex,
    };
    std::vector<VkDeviceQueueCreateInfo> qci(indices.size());
    uint32_t i = 0;
    float prio = 1.0f;
    for (uint32_t index : indices) {
        qci[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci[i].queueCount = 1;
        qci[i].queueFamilyIndex = index;
        qci[i].pQueuePriorities = &prio;
        i++;
    }
    std::vector<const char*> enabledLayerNames {
#if !defined(NDEBUG)
        "VK_LAYER_LUNARG_standard_validation",
#endif
    };
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
    createInfo.ppEnabledLayerNames = enabledLayerNames.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(sd.enabledExtensionNames.size());
    createInfo.ppEnabledExtensionNames = sd.enabledExtensionNames.data();
    createInfo.pEnabledFeatures = &sd.features;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(qci.size());
    createInfo.pQueueCreateInfos = qci.data();
    vkcall(vkCreateDevice(sd.physicalDevice, &createInfo, nullptr, &ctx.device));
#define VKF_DEVICE(func)                                                         \
    func = reinterpret_cast<PFN_##func>(vkGetDeviceProcAddr(ctx.device, #func)); \
    expect(func != nullptr);
#include "device.inl"
    ctx.physicalDevice = sd.physicalDevice;
    ctx.renderQueueFamilyIndex = sd.renderQueueFamilyIndex;
    ctx.transferQueueFamilyIndex = sd.transferQueueFamilyIndex;
    vkGetDeviceQueue(ctx.device, ctx.renderQueueFamilyIndex, 0, &ctx.renderQueue);
    vkGetDeviceQueue(ctx.device, ctx.transferQueueFamilyIndex, 0, &ctx.transferQueue);
}

void destroyDevice() {
    if (ctx.device) {
        vkDestroyDevice(ctx.device, nullptr);
    }
}
