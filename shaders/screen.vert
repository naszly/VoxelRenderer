#version 330

layout(location = 0) in uint aPackedPosition;
layout(location = 1) in uint aPackedColor;
layout(location = 2) in uint aTextureIndex;

uniform mat4 uProjectionView;
uniform vec3 uCameraPosition;

uniform vec2 uViewportSize;

uniform vec3 uChunkPosition;
uniform float uChunkSize;

out vec3 vPosition;
out vec3 vColor;
flat out uint vTextureIndex;

// Fast Quadric Proj: "GPU-Based Ray-Casting of Quadratic Surfaces" http://dl.acm.org/citation.cfm?id=2386396
void quadricProj(in vec3 osPosition, in float voxelSize, in mat4 objectToScreenMatrix, in vec2 screenSize,
inout vec4 position, inout float pointSize) {
    const vec4 quadricMat = vec4(1.0, 1.0, 1.0, -1.0);
    float sphereRadius = voxelSize * 1.732051;
    vec4 sphereCenter = vec4(osPosition.xyz, 1.0);
    mat4 modelViewProj = transpose(objectToScreenMatrix);
    mat3x4 matT = mat3x4(mat3(modelViewProj[0].xyz, modelViewProj[1].xyz, modelViewProj[3].xyz) * sphereRadius);
    matT[0].w = dot(sphereCenter, modelViewProj[0]);
    matT[1].w = dot(sphereCenter, modelViewProj[1]);
    matT[2].w = dot(sphereCenter, modelViewProj[3]);
    mat3x4 matD = mat3x4(matT[0] * quadricMat, matT[1] * quadricMat, matT[2] * quadricMat);
    vec4 eqCoefs = vec4(dot(matD[0], matT[2]),
                        dot(matD[1], matT[2]),
                        dot(matD[0], matT[0]),
                        dot(matD[1], matT[1])) / dot(matD[2], matT[2]);
    vec4 outPosition = vec4(eqCoefs.x, eqCoefs.y, 0.0, 1.0);
    vec2 AABB = sqrt(eqCoefs.xy*eqCoefs.xy - eqCoefs.zw);
    AABB *= screenSize;
    position.xy = outPosition.xy * position.w;
    pointSize = max(AABB.x, AABB.y);
}

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
    vPosition = unpackPosition(aPackedPosition) + vec3(0.5) + uChunkPosition * uChunkSize - uCameraPosition;
    vColor = unpackUnorm4x8(aPackedColor).xyz;
    vTextureIndex = aTextureIndex;

    vec4 position = uProjectionView * vec4(vPosition, 1.0);

    float pointSize;

    quadricProj(vPosition.xyz, 0.5, uProjectionView, uViewportSize, position, pointSize);

    gl_Position = position;
    gl_PointSize = pointSize;
}