#version 330

#define UINT_MAX 4294967295u

in vec3 vPosition;
in vec3 vColor;
flat in uint vTextureIndex;

uniform sampler2DArray uTextures;

uniform mat4 uInvProjectionView;

uniform vec2 uViewportSize;
uniform float uNearPlane;
uniform float uFarPlane;

uniform float uReach;

out vec4 outColor;

struct Box {
    vec3     center;
    vec3     radius;
    vec3     invRadius;
    mat3     rotation;
};

struct Ray {
    vec3     origin;
    vec3     direction;
};

float maxComponent(vec3 a) {
    return max(a.x, max(a.y, a.z));
}

// vec3 box.radius:       independent half-length along the X, Y, and Z axes
// mat3 box.rotation:     box-to-world rotation (orthonormal 3x3 matrix) transformation
// bool rayCanStartInBox: if true, assume the origin is never in a box. GLSL optimizes this at compile time
// bool oriented:         if false, ignore box.rotation
bool intersectBox(Box box, Ray ray, out float distance, out vec3 normal, out vec2 texCoord, const bool rayCanStartInBox, const in bool oriented) {

    vec3 invRayDirection = 1.0 / ray.direction;

    // Move to the box's reference frame. This is unavoidable and un-optimizable.
    ray.origin = box.rotation * (ray.origin - box.center);
    if (oriented) {
        ray.direction = ray.direction * box.rotation;
    }

    // This "rayCanStartInBox" branch is evaluated at compile time because `const` in GLSL
    // means compile-time constant. The multiplication by 1.0 will likewise be compiled out
    // when rayCanStartInBox = false.
    float winding;
    if (rayCanStartInBox) {
        // Winding direction: -1 if the ray starts inside of the box (i.e., and is leaving), +1 if it is starting outside of the box
        winding = (maxComponent(abs(ray.origin) * box.invRadius) < 1.0) ? -1.0 : 1.0;
    } else {
        winding = 1.0;
    }

    // We'll use the negated sign of the ray direction in several places, so precompute it.
    // The sign() instruction is fast...but surprisingly not so fast that storing the result
    // temporarily isn't an advantage.
    vec3 sgn = -sign(ray.direction);

    // Ray-plane intersection. For each pair of planes, choose the one that is front-facing
    // to the ray and compute the distance to it.
    vec3 distanceToPlane = box.radius * winding * sgn - ray.origin;
    if (oriented) {
        distanceToPlane /= ray.direction;
    } else {
        distanceToPlane *= invRayDirection;
    }

    // Perform all three ray-box tests and cast to 0 or 1 on each axis.
    // Use a macro to eliminate the redundant code (no efficiency boost from doing so, of course!)
    // Could be written with
    #   define TEST(U, VW)\
         /* Is there a hit on this axis in front of the origin? Use multiplication instead of && for a small speedup */\
         (distanceToPlane.U >= 0.0) && \
         /* Is that hit within the face of the box? */\
         all(lessThan(abs(ray.origin.VW + ray.direction.VW * distanceToPlane.U), box.radius.VW))

    bvec3 test = bvec3(TEST(x, yz), TEST(y, zx), TEST(z, xy));

    // CMOV chain that guarantees exactly one element of sgn is preserved and that the value has the right sign
    sgn = test.x ? vec3(sgn.x, 0.0, 0.0) : (test.y ? vec3(0.0, sgn.y, 0.0) : vec3(0.0, 0.0, test.z ? sgn.z : 0.0));
    #   undef TEST

    // At most one element of sgn is non-zero now. That element carries the negative sign of the
    // ray direction as well. Notice that we were able to drop storage of the test vector from registers,
    // because it will never be used again.

    // Mask the distance by the non-zero axis
    // Dot product is faster than this CMOV chain, but doesn't work when distanceToPlane contains nans or infs.
    //
    distance = (sgn.x != 0.0) ? distanceToPlane.x : ((sgn.y != 0.0) ? distanceToPlane.y : distanceToPlane.z);

    // Normal must face back along the ray. If you need
    // to know whether we're entering or leaving the box,
    // then just look at the value of winding. If you need
    // texture coordinates, then use box.invDirection * hitPoint.

    #define GET_TEXCOORD(UV)\
        ((ray.origin.UV + ray.direction.UV * distance + box.radius.UV) * box.invRadius.UV * 0.5)

    if (sgn.x > 0)
        texCoord = GET_TEXCOORD(zy) * vec2(-1.0, 1.0) + vec2(1.0, 0.0);
    else if (sgn.x < 0)
        texCoord = GET_TEXCOORD(zy);
    else if (sgn.y > 0)
        texCoord = GET_TEXCOORD(xz) * vec2(-1.0, 1.0) + vec2(1.0, 0.0);
    else if (sgn.y < 0)
        texCoord = GET_TEXCOORD(xz);
    else if (sgn.z > 0)
        texCoord = GET_TEXCOORD(xy);
    else
        texCoord = GET_TEXCOORD(xy) * vec2(-1.0, 1.0) + vec2(1.0, 0.0);

    #undef GET_TEXCOORD

    if (oriented) {
        normal = box.rotation * sgn;
    } else {
        normal = sgn;
    }

    return (sgn.x != 0) || (sgn.y != 0) || (sgn.z != 0);
}

vec3 screenToWorldSpace(vec2 screenPosition){
    screenPosition = screenPosition * 2.0 - 1.0;
    return vec3(uInvProjectionView * vec4(screenPosition, -1.0, 1.0));
}

void main(void) {
    vec3 light = normalize(vec3(0.3, 1.0, -0.5));

    Ray ray;
    ray.origin = vec3(0.0, 0.0, 0.0);
    ray.direction = screenToWorldSpace(gl_FragCoord.xy / uViewportSize);

    Box box;
    box.center = vPosition;
    box.radius = vec3(0.5, 0.5, 0.5);
    box.invRadius = 1.0 / box.radius;
    box.rotation = mat3(1.0);

    float distance;
    vec3 normal;
    vec2 textureCoord;

    if (intersectBox(box, ray, distance, normal, textureCoord, true, true)) {
        // overwriting the depth buffer breaks the early depth test
        // causing significant performance loss
        // gl_FragDepth = (distance - uNearPlane) / (uFarPlane - uNearPlane);

        // calculate the color and lighting
        vec3 color = vColor;
        vec3 lightColor = vec3(1.0, 1.0, 1.0);
        vec3 ambient = vec3(0.1, 0.1, 0.1);
        vec3 diffuse = lightColor * max(dot(normal, light), 0.0);
        vec3 specular = vec3(0.0, 0.0, 0.0);
        vec3 finalColor = color * (ambient + diffuse + specular);
        outColor = vec4(finalColor, 1.0);

        if (vTextureIndex != UINT_MAX)
            outColor *= texture(uTextures, vec3(textureCoord, vTextureIndex));

        // draw a circular crosshair mid-screen
        vec2 aspect = vec2(uViewportSize.x / uViewportSize.y, 1.0);
        if (length((gl_FragCoord.xy / uViewportSize * aspect) - (aspect * 0.5)) < 0.005) {
            if (distance < uReach) {
                outColor = mix(vec4(0.0, 1.0, 0.2, 1.0), outColor, 0.5);
            } else {
                outColor = mix(vec4(1.0, 0.0, 0.2, 1.0), outColor, 0.5);
            }
        }

    } else {
        discard;
    }
}
