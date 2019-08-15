#include "buffer.h"

void TBufferPool::create() {
    m_createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    m_createInfo.size = m_bufferCount * m_bufferCount;
    m_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkcall(vkCreateBuffer(ctx.device, &m_createInfo, nullptr, &m_buffer));
    VkMemoryRequirements reqs;
    vkGetBufferMemoryRequirements(ctx.device, m_buffer, &reqs);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = reqs.size;
    allocInfo.memoryTypeIndex = ctx.findMemoryIndex(reqs.memoryTypeBits, m_memoryProps);
    vkcall(vkAllocateMemory(ctx.device, &allocInfo, nullptr, &m_memory));
    vkcall(vkBindBufferMemory(ctx.device, m_buffer, m_memory, 0));
    m_buffers.reserve(m_bufferCount);
    for (uint32_t i = 0; i < m_bufferCount; i++) {
        m_buffers.push_back(Buffer{m_buffer, i * m_bufferSize, this});
    }
    for (uint32_t i = 0; i < m_bufferCount - 1; i++) {
        m_buffers[i].m_nextBuffer = &m_buffers[i + 1];
    }
    m_firstBuffer = &m_buffers[0];
}

void TBufferPool::destroy() {
    if (m_buffer)
        vkDestroyBuffer(ctx.device, m_buffer, nullptr);
    if (m_memory)
        vkFreeMemory(ctx.device, m_memory, nullptr);
}

Buffer* TBufferPool::allocate() {
    Expect(m_firstBuffer);
    Buffer* buffer = m_firstBuffer;
    m_firstBuffer = buffer->m_nextBuffer;
    buffer->m_nextBuffer = nullptr;
    return buffer;
}

void Buffer::free() {
    m_nextBuffer = m_pool->m_firstBuffer;
    m_pool->m_firstBuffer = this;
}
