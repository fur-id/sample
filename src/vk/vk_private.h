#pragma once

#include "vk.h"

extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

#define VKF_GLOBAL(func) extern PFN_##func func;
#include "global.inl"
#undef VKF_GLOBAL
