#include "vk/vk.h"

#include "vmath.h"
#include "vk/image.h"

#include <array>

struct TPush128 {
    struct {
        mat4 modelMatrix;
    } vert;
    struct {
        uint32_t textureIndex;
    } frag;
};

void sample() {
    VkCommandBuffer cmd = VK_NULL_HANDLE;
    VkPipelineLayout playout = VK_NULL_HANDLE;

    // starting game
    TImagePool texturePool;
    texturePool
        .width(512)
        .height(512)
        .count(256)
        .mipLevels(3)
        .format(VK_FORMAT_BC7_UNORM_BLOCK)
        .sampledImage()
        .transferDestination()
        .deviceLocal()
        .create();

    // when new texture needed
    TImage* texture = texturePool.allocate();

    // when recording command buffer
    TPush128 push;
    push.vert.modelMatrix = mat4{};
    push.frag.textureIndex = texture->layer;
    vkCmdPushConstants(cmd,
                       playout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       offsetof(TPush128, vert),
                       sizeof(TPush128::vert),
                       &push.vert);
    vkCmdPushConstants(cmd,
                       playout,
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       offsetof(TPush128, frag),
                       sizeof(TPush128::frag),
                       &push.frag);

    // when texture is not needed
    texture->free();

    // closing game
    texturePool.destroy();
}
