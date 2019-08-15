#include "vk_private.h"

#include <string>

#if defined(_WIN64)
#include <Windows.h>
static HMODULE vklib{NULL};
static void loadVulkanLibrary() {
    vklib = LoadLibrary("vulkan-1.dll");
    expect(vklib != nullptr);
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(vklib, "vkGetInstanceProcAddr"));
    expect(vkGetInstanceProcAddr != nullptr);
}
static void freeVulkanLibrary() {
    FreeLibrary(vklib);
}
#elif defined(__linux__)
#include <dlfcn.h>
static void* vklib{nullptr};
static void loadVulkanLibrary() {
    vklib = dlopen("libvulkan.so.1", RTLD_NOW);
    expect(vklib != nullptr);
    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(vklib, "vkGetInstanceProcAddr"));
    expect(vkGetInstanceProcAddr != nullptr);
}
static void freeVulkanLibrary() {
    dlclose(vklib);
}
#endif

#define VKF_GLOBAL(func)                                                        \
    func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(nullptr, #func)); \
    expect(func != nullptr);

void createInstance(std::vector<const char*>& requiredExtensionNames) {
    loadVulkanLibrary();
#include "global.inl"
#undef VKF_GLOBAL
    uint32_t count;
    vkcall(vkEnumerateInstanceLayerProperties(&count, nullptr));
    std::vector<VkLayerProperties> availableLayers(count);
    vkcall(vkEnumerateInstanceLayerProperties(&count, availableLayers.data()));
    std::vector<const char*> requiredLayerNames{
        "VK_LAYER_LUNARG_standard_validation",
    };
    std::vector<const char*> enabledLayerNames;
    for (const char* name : requiredLayerNames) {
        bool found = true;
        for (VkLayerProperties& props : availableLayers) {
            if (std::string(name) == props.layerName) {
                found = true;
                enabledLayerNames.push_back(name);
                break;
            }
        }
        expect(found);
    }
    vkcall(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
    std::vector<VkExtensionProperties> availableExtensions(count);
    vkcall(vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data()));
    std::vector<const char*> enabledExtensionNames;
    for (const char* name : requiredExtensionNames) {
        bool found = true;
        for (VkExtensionProperties& props : availableExtensions) {
            if (std::string(name) == props.extensionName) {
                found = true;
                enabledExtensionNames.push_back(name);
                break;
            }
        }
        expect(found);
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Anime RTS";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "Anime";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
    createInfo.ppEnabledLayerNames = enabledLayerNames.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
    createInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
    vkcall(vkCreateInstance(&createInfo, nullptr, &ctx.instance));
#define VKF_INSTANCE(func)                                                           \
    func = reinterpret_cast<PFN_##func>(vkGetInstanceProcAddr(ctx.instance, #func)); \
    expect(func != nullptr);
#include "instance.inl"
}

void destroyInstance() {
    if (ctx.surface) {
        vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    }
    if (ctx.instance) {
        vkDestroyInstance(ctx.instance, nullptr);
    }
    freeVulkanLibrary();
}
