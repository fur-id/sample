#include "engine/vulkan/vk_private.h"

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{nullptr};

#define VKF_GLOBAL(func) PFN_##func func{nullptr};
#include "global.inl"
#undef VKF_GLOBAL

#define VKF_INSTANCE(func) PFN_##func func{nullptr};
#include "instance.inl"

#define VKF_DEVICE(func) PFN_##func func{nullptr};
#include "device.inl"

TContext ctx;
