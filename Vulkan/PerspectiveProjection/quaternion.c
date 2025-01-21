#include "quaternion.h"
#define _USE_MATH_DEFINES
#include <math.h>

float degrees(float radians) {
    return radians * (180.0f / M_PI);
}

float radians(float degrees) {
    return degrees * M_PI / 180.0f;
}

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
    float s = sinf(angle / 2.0f);

    return (Quaternion) {
        .w = cosf(angle / 2.0f),
        .x = s * axis.x,
        .y = s * axis.y,
        .z = s * axis.z
    };
}

Vector3 quaternion_rotate_vector(Quaternion q, Vector3 v) {
    Quaternion result = quaternion_multiply(quaternion_multiply(q, (Quaternion) { .w = 0.0f, .x = v.x, .y = v.y, .z = v.z }), quaternion_conjugate(q));

    return (Vector3) { .x = result.x, .y = result.y, .z = result.z };
}

float quaternion_extract_roll_angle(Quaternion q) {
    return atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
}

float quaternion_extract_yaw_angle(Quaternion q) {
    return atan2f(2.0f * (q.w * q.y + q.x * q.z), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
}

float quaternion_extract_pitch_angle(Quaternion q) {
    return asinf(2.0f * (q.w * q.x - q.y * q.z));
}

Quaternion quaternion_slerp(Quaternion a, Quaternion b, float t) {
    float dot = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;

    if (dot < 0.0f) {
        b.w = -b.w;
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        dot = -dot;
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        Quaternion result = {
            .w = a.w + t * (b.w - a.w),
            .x = a.x + t * (b.x - a.x),
            .y = a.y + t * (b.y - a.y),
            .z = a.z + t * (b.z - a.z)
        };
        return quaternion_normalize(result);
    }

    float theta_0 = acosf(dot);  // theta_0 = angle between input vectors
    float theta = theta_0 * t;   // theta = angle between a and result
    float sin_theta = sinf(theta);
    float sin_theta_0 = sinf(theta_0);

    float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;  // = sin(theta_0 - theta) / sin(theta_0)
    float s1 = sin_theta / sin_theta_0;

    return (Quaternion) {
        .w = s0 * a.w + s1 * b.w,
        .x = s0 * a.x + s1 * b.x,
        .y = s0 * a.y + s1 * b.y,
        .z = s0 * a.z + s1 * b.z
    };
}
