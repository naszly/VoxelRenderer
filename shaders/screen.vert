#version 450

#define MAX_MATERIALS 1024u

layout(location = 0) in vec3 aBillboardPosition;
layout(location = 1) in uint aPackedVoxelPosition;
layout(location = 2) in uint aMaterialIndex;

struct Material {
    vec4 color;
    uint texture;
};

layout(binding = 0) uniform uMaterials {
    Material materials[MAX_MATERIALS];
};

uniform mat4 uProjectionView;
uniform vec3 uCameraPosition;

uniform vec3 uChunkPosition;
uniform float uChunkSize;

out vec3 vPosition;
out vec3 vColor;

vec3 unpackPosition(in uint packedPosition) {
    // x 10 bits, y 10 bits, z 10 bits
    vec3 position;
    position.x = float((packedPosition >> 20) & 0x3FFu);
    position.y = float((packedPosition >> 10) & 0x3FFu);
    position.z = float(packedPosition & 0x3FFu);
    return position;
}

vec4 unpackUnorm4x8(in uint packedColor) {
    vec4 color;
    color.r = float((packedColor >> 24) & 0xFFu) / 255.0;
    color.g = float((packedColor >> 16) & 0xFFu) / 255.0;
    color.b = float((packedColor >> 8) & 0xFFu) / 255.0;
    color.a = float(packedColor & 0xFFu) / 255.0;

    return color;
}

void main(void) {
    vec3 voxelPosition = unpackPosition(aPackedVoxelPosition) + uChunkPosition * uChunkSize - uCameraPosition;
    vec3 voxelColor = materials[aMaterialIndex].color.xyz;

    vec3 viewDir = normalize(-voxelPosition);
    vec3 right = normalize(cross(vec3(0, 1, 0), viewDir));
    vec3 up = normalize(cross(viewDir, right));

    vec3 billboardPos = (voxelPosition + aBillboardPosition.x * right + aBillboardPosition.y * up);

    gl_Position = uProjectionView * vec4(billboardPos, 1.0);

    vPosition = voxelPosition;
    vColor = voxelColor;
}