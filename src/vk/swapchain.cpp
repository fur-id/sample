#include "swapchain.h"

#include <limits>

void Swapchain::setExtent(uint32_t width, uint32_t height) {
    destroy();

    VkSurfaceCapabilitiesKHR caps;
    vkcall(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.physicalDevice, ctx.surface, &caps));

    if (caps.currentExtent.width == std::numeric_limits<decltype(caps.currentExtent.width)>::max()) { // any size
        m_extent.width = width;
        m_extent.height = height;
    } else {
        m_extent = caps.currentExtent;
    }

    uint32_t count;
    vkcall(vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physicalDevice, ctx.surface, &count, nullptr));
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkcall(vkGetPhysicalDeviceSurfaceFormatsKHR(ctx.physicalDevice, ctx.surface, &count, formats.data()));
    if (formats.size() == 1 and formats[0].format == VK_FORMAT_UNDEFINED) {
        m_surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        m_surfaceFormat.format = VK_FORMAT_B8G8R8A8_SRGB;
    } else {
        for (VkSurfaceFormatKHR& format : formats) {
            if (format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR and format.format == VK_FORMAT_B8G8R8A8_SRGB) {
                m_surfaceFormat = format;
                break;
            }
        }
        if (m_surfaceFormat.format == VK_FORMAT_UNDEFINED) {
            m_surfaceFormat = formats[0];
        }
    }
    vkcall(vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physicalDevice, ctx.surface, &count, nullptr));
    std::vector<VkPresentModeKHR> modes(count);
    vkcall(vkGetPhysicalDeviceSurfacePresentModesKHR(ctx.physicalDevice, ctx.surface, &count, modes.data()));
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    m_imageCount = 2;
    for (VkPresentModeKHR mode : modes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = mode;
            m_imageCount = 3;
            break;
        }
    }
    if (caps.maxImageCount != 0) {
        m_imageCount = std::max(m_imageCount, caps.minImageCount);
        m_imageCount = std::min(m_imageCount, caps.maxImageCount);
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.surface = ctx.surface;
    createInfo.imageUsage = VK_IMAGE_USAGE_STORAGE_BIT; // result of postprocessing
    createInfo.imageFormat = m_surfaceFormat.format;
    createInfo.presentMode = presentMode;
    createInfo.preTransform = caps.currentTransform;
    createInfo.minImageCount = m_imageCount;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkcall(vkCreateSwapchainKHR(ctx.device, &createInfo, nullptr, &m_swapchain));

    vkcall(vkGetSwapchainImagesKHR(ctx.device, m_swapchain, &m_imageCount, nullptr));
    m_images.resize(m_imageCount);
    vkcall(vkGetSwapchainImagesKHR(ctx.device, m_swapchain, &m_imageCount, m_images.data()));

    m_views.resize(m_imageCount);
    for (VkImageView& view : m_views) {
        view = VK_NULL_HANDLE;
    }
    for (uint32_t i = 0; i < m_imageCount; i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        createInfo.format = m_surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        vkcall(vkCreateImageView(ctx.device, &createInfo, nullptr, &m_views[i]));
    }
}

void Swapchain::destroy() {
    for (VkImageView& view : m_views) {
        vkDestroyImageView(ctx.device, view, nullptr);
        view = VK_NULL_HANDLE;
    }
    vkDestroySwapchainKHR(ctx.device, m_swapchain, nullptr);
}
