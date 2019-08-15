#include "image.h"

void TImagePool::create() {
    m_createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    m_createInfo.extent.depth = 1;
    m_createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    m_createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    m_createInfo.imageType = VK_IMAGE_TYPE_2D;
    m_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    m_createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkcall(vkCreateImage(ctx.device, &m_createInfo, nullptr, &m_image));
    vkGetImageMemoryRequirements(ctx.device, m_image, &m_reqs);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = m_reqs.size;
    allocInfo.memoryTypeIndex = ctx.findMemoryIndex(m_reqs.memoryTypeBits, m_memoryProps);
    vkcall(vkAllocateMemory(ctx.device, &allocInfo, nullptr, &m_memory));
    vkcall(vkBindImageMemory(ctx.device, m_image, m_memory, 0));
    m_images.reserve(m_createInfo.arrayLayers);
    for (uint32_t i = 0; i < m_createInfo.arrayLayers; i++) {
        m_images.push_back(TImage{m_image, i, this});
    }
    for (uint32_t i = 0; i < m_createInfo.arrayLayers - 1; i++) {
        m_images[i].m_nextImage = &m_images[i + 1];
    }
    m_firstImage = &m_images[0];
}

void TImagePool::destroy() {
    if (m_image) {
        vkDestroyImage(ctx.device, m_image, nullptr);
    }
    if (m_memory) {
        vkFreeMemory(ctx.device, m_memory, nullptr);
    }
}

TImage* TImagePool::allocate() {
    expect(m_firstImage);
    TImage* image = m_firstImage;
    m_firstImage = image->m_nextImage;
    image->m_nextImage = nullptr;
    return image;
}

void TImage::free() {
    m_nextImage = m_pool->m_firstImage;
    m_pool->m_firstImage = this;
}
