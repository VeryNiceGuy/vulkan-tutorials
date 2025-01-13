#include "quaternion.h"
#define _USE_MATH_DEFINES
#include <math.h>

Quaternion quaternion_add(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .w = a.w + b.w,
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
}

Quaternion quaternion_subtract(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .w = a.w - b.w,
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
}

Quaternion quaternion_multiply(Quaternion a, Quaternion b) {
    return (Quaternion) {
        .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        .x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        .y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        .z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
    };
}

Quaternion quaternion_scale(Quaternion q, float s) {
    return (Quaternion) {
        .w = q.w * s,
        .x = q.x * s,
        .y = q.y * s,
        .z = q.z * s
    };
}

float quaternion_magnitude(Quaternion q) {
    return sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
}

Quaternion quaternion_normalize(Quaternion q) {
    float mag = quaternion_magnitude(q);

    return (Quaternion) {
        .w = q.w / mag,
        .x = q.x / mag,
        .y = q.y / mag,
        .z = q.z / mag };
}

Quaternion quaternion_conjugate(Quaternion q) {
    return (Quaternion) {
        .w = q.w,
        .x = -q.x,
        .y = -q.y,
        .z = -q.z
    };
}

Quaternion quaternion_inverse(Quaternion q) {
    Quaternion conjugated = quaternion_conjugate(q);
    float magSquared = quaternion_magnitude(q) * quaternion_magnitude(q);

    return (Quaternion) {
        .w = conjugated.w / magSquared,
        .x = conjugated.x / magSquared,
        .y = conjugated.y / magSquared,
        .z = conjugated.z / magSquared
    };
}

Quaternion quaternion_angle_axis(float angle, Vector3 axis) {
    float rad = angle * M_PI / 180.0f;
    float s = sinf(rad / 2.0f);

    return (Quaternion) {
        .w = cosf(rad / 2.0f),
        .x = s * axis.x,
        .y = s * axis.y,
        .z = s * axis.z
    };
}

Vector3 quaternion_rotate_vector(Quaternion q, Vector3 v) {
    Quaternion result = quaternion_multiply(quaternion_multiply(q, (Quaternion) { .w = 0.0f, .x = v.x, .y = v.y, .z = v.z }), quaternion_conjugate(q));

    return (Vector3) { .x = result.x, .y = result.y, .z = result.z };
}