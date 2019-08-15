#version 450

layout(set=0, binding=0) uniform sampler2DArray texturePool;

layout(location=0) out vec4 outColor;

layout(push_constant) uniform FragData {
    layout(offset=64) // sizeof(TPush128::vert)
    uint textureIndex;
} fragData;

void main() {
    outColor = vec4(1, 1, 0, 1);
}
