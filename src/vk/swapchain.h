#pragma once

#include "vk.h"

class Swapchain {
public:
    void setExtent(uint32_t width, uint32_t height);
    void destroy();

private:
    VkSwapchainKHR m_swapchain{VK_NULL_HANDLE};
    uint32_t m_imageCount;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_views;
    uint32_t m_currentImageIndex;
    VkSurfaceFormatKHR m_surfaceFormat{};
    VkExtent2D m_extent;
};
