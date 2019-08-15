#pragma once

#include "vk.h"

class TImagePool;

struct TImage {
    const VkImage image;
    const uint32_t layer;
    TImage(VkImage image, uint32_t layer, TImagePool* pool)
        : image(image), layer(layer), m_pool(pool) {}
    void free();

private:
    friend class TImagePool;
    TImage* m_nextImage{nullptr};
    TImagePool* m_pool;
};

class TImagePool {
public:
    VkDeviceSize byteSize() const { return m_reqs.size; }
    uint32_t width() const { return m_createInfo.extent.width; }
    TImagePool& width(uint32_t width) {
        m_createInfo.extent.width = width;
        return *this;
    }
    uint32_t height() const { return m_createInfo.extent.height; }
    TImagePool& height(uint32_t height) {
        m_createInfo.extent.height = height;
        return *this;
    }
    uint32_t count() const { return m_createInfo.arrayLayers; }
    TImagePool& count(uint32_t count) {
        m_createInfo.arrayLayers = count;
        return *this;
    }
    TImagePool& mipLevels(uint32_t mipLevels) {
        m_createInfo.mipLevels = mipLevels;
        return *this;
    }
    VkFormat format() const { return m_createInfo.format; }
    TImagePool& format(VkFormat format) {
        m_createInfo.format = format;
        return *this;
    }
    TImagePool& sampledImage() {
        m_createInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        return *this;
    }
    TImagePool& storageImage() {
        m_createInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        return *this;
    }
    TImagePool& colorAttachment() {
        m_createInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        return *this;
    }
    TImagePool& depthStencilAttachment() {
        m_createInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        return *this;
    }
    TImagePool& transferSource() {
        m_createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        return *this;
    }
    TImagePool& transferDestination() {
        m_createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        return *this;
    }
    TImagePool& deviceLocal() {
        m_memoryProps |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        return *this;
    }
    TImagePool& hostVisible() {
        m_memoryProps |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        return *this;
    }
    TImagePool& hostCoherent() {
        m_memoryProps |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        return *this;
    }
    void create();
    void destroy();

    TImage* allocate();

private:
    friend struct TImage;
    VkImageCreateInfo m_createInfo{};
    VkMemoryPropertyFlags m_memoryProps{0};
    VkImage m_image{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    std::vector<TImage> m_images;
    TImage* m_firstImage{nullptr};
    VkMemoryRequirements m_reqs;
};
