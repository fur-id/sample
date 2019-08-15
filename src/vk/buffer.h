#pragma once

#include "vk.h"

class TBufferPool;

struct Buffer {
    const VkBuffer buffer;
    const VkDeviceSize offset;
    Buffer(VkBuffer buffer, VkDeviceSize offset, TBufferPool* pool)
        : buffer(buffer), offset(offset), m_pool(pool) {}
    void free();

private:
    friend class TBufferPool;
    Buffer* m_nextBuffer;
    TBufferPool* m_pool;
};

class TBufferPool {
public:
    TBufferPool& bufferCount(uint32_t count) {
        m_bufferCount = count;
        return *this;
    }
    TBufferPool& bufferSize(VkDeviceSize size) {
        m_bufferSize = size;
        return *this;
    }
    TBufferPool& vertexBuffer() {
        m_createInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        return *this;
    }
    TBufferPool& indexBuffer() {
        m_createInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        return *this;
    }
    TBufferPool& uniformBuffer() {
        m_createInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        return *this;
    }
    TBufferPool& storageBuffer() {
        m_createInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        return *this;
    }
    TBufferPool& transferSource() {
        m_createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        return *this;
    }
    TBufferPool& transferDestination() {
        m_createInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        return *this;
    }
    TBufferPool& deviceLocal() {
        m_memoryProps |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        return *this;
    }
    TBufferPool& hostVisible() {
        m_memoryProps |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        return *this;
    }
    TBufferPool& hostCoherent() {
        m_memoryProps |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        return *this;
    }
    void create();
    void destroy();

    Buffer* allocate();

private:
    friend struct Buffer;
    uint32_t m_bufferCount{1};
    VkDeviceSize m_bufferSize{1};
    VkBufferCreateInfo m_createInfo{};
    VkMemoryPropertyFlags m_memoryProps{0};
    VkBuffer m_buffer{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    std::vector<Buffer> m_buffers;
    Buffer* m_firstBuffer{nullptr};
};
