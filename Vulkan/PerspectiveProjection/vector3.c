#include "vector3.h"
#include <math.h>

Vector3 vector3_normalize(Vector3 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

    return (Vector3) {
        .x = v.x / length,
        .y = v.y / length,
        .z = v.z / length
    };
}

float vector3_magnitude(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 vector3_add(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
}

Vector3 vector3_add_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x + s,
        .y = v.y + s,
        .z = v.z + s
    };
}

Vector3 vector3_subtract(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
}

Vector3 vector3_subtract_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x - s,
        .y = v.y - s,
        .z = v.z - s
    };
}

Vector3 vector3_multiply(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z
    };
}

Vector3 vector3_scale(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
}

Vector3 vector3_divide(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = a.x / b.x,
        .y = a.y / b.y,
        .z = a.z / b.z
    };
}

Vector3 vector3_divide_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
}

Vector3 vector3_cross(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
}

float vector3_dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float vector3_distance_to(Vector3 a, Vector3 b) {
    return sqrtf(
        (a.x - b.x) * (a.x - b.x)
        + (a.y - b.y) * (a.y - b.y)
        + (a.z - b.z) * (a.z - b.z));
}

float vector3_angle_between(Vector3 a, Vector3 b) {
    float dotProduct = vector3_dot(a, b);
    float magnitudes = vector3_magnitude(a) * vector3_magnitude(b);

    return acosf(dotProduct / magnitudes);
}

float vector3_angle_to(Vector3 a, Vector3 b) {
    return atan2f(vector3_magnitude(vector3_cross(a, b)), vector3_dot(a, b));
}

Vector3 vector3_reflect(Vector3 v, Vector3 normal) {
    return vector3_subtract(v, vector3_scale(normal, 2 * vector3_dot(v, normal)));
}

Vector3 vector3_lerp(Vector3 a, Vector3 b, float t) {
    return vector3_add(vector3_scale(a, 1.0f - t), vector3_scale(b, t));
}

Vector3 vector3_clamp(Vector3 v, Vector3 min, Vector3 max) {
    return (Vector3) {
        .x = fmaxf(min.x, fminf(v.x, max.x)),
        .y = fmaxf(min.y, fminf(v.y, max.y)),
        .z = fmaxf(min.z, fminf(v.z, max.z))
    };
}

Vector3 vector3_midpoint(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = (a.x + b.x) / 2.0f,
        .y = (a.y + b.y) / 2.0f,
        .z = (a.z + b.z) / 2.0f
    };
}

Vector3 vector3_project_onto(Vector3 a, Vector3 b) {
    float dotProduct = vector3_dot(a, b);
    float otherMagnitudeSquared = vector3_magnitude(b) * vector3_magnitude(b);

    return vector3_scale(b, dotProduct / otherMagnitudeSquared);
}

Vector3 vector3_perpendicular_to(Vector3 a, Vector3 b) {
    return vector3_subtract(a, vector3_project_onto(a, b));
}

float vector3_triple_product(Vector3 a, Vector3 b, Vector3 c) {
    return vector3_dot(a, vector3_cross(b, c));
}

Vector3 vector3_rotate_around_axis(Vector3 v, Vector3 axis, float angle) {
    Vector3 normalizedAxis = vector3_normalize(axis);
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return vector3_add(
        vector3_add(vector3_scale(v, cosAngle), vector3_scale(vector3_cross(normalizedAxis, v), sinAngle)),
        vector3_scale(normalizedAxis, vector3_dot(normalizedAxis, v) * (1.0f - cosAngle))
    );
}

Vector3 vector3_rotate_x(Vector3 v, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return (Vector3) {
        .x = v.x,
        .y = v.y * cosAngle - v.z * sinAngle,
        .z = v.y * sinAngle + v.z * cosAngle
    };
}

Vector3 vector3_rotate_y(Vector3 v, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return (Vector3) {
        .x = v.x * cosAngle + v.z * sinAngle,
        .y = v.y,
        .z = -v.x * sinAngle + v.z * cosAngle
    };
}

Vector3 vector3_rotate_z(Vector3 v, float angle) {
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return (Vector3) {
        .x = v.x * cosAngle - v.y * sinAngle,
        .y = v.x * sinAngle + v.y * cosAngle,
        .z = v.z
    };
}

Vector3 vector3_negate(Vector3 v) {
    return (Vector3) {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z
    };
}

Vector3 vector3_abs(Vector3 v) {
    return (Vector3) {
        .x = fabsf(v.x),
        .y = fabsf(v.y),
        .z = fabsf(v.z)
    };
}

float vector3_max_component(Vector3 v) {
    return fmaxf(fmaxf(v.x, v.y), v.z);
}

float vector3_min_component(Vector3 v) {
    return fminf(fminf(v.x, v.y), v.z);
}

Vector3 vector3_pow(Vector3 v, float exponent) {
    return (Vector3) {
        .x = powf(v.x, exponent),
        .y = powf(v.y, exponent),
        .z = powf(v.z, exponent)
    };
}

Vector3 vector3_sqrt(Vector3 v) {
    return (Vector3) {
        .x = sqrtf(v.x),
        .y = sqrtf(v.y),
        .z = sqrtf(v.z)
    };
}

Vector3 vector3_min(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = fminf(a.x, b.x),
        .y = fminf(a.y, b.y),
        .z = fminf(a.z, b.z)
    };
}

Vector3 vector3_max(Vector3 a, Vector3 b) {
    return (Vector3) {
        .x = fmaxf(a.x, b.x),
        .y = fmaxf(a.y, b.y),
        .z = fmaxf(a.z, b.z)
    };
}

Vector3 vector3_round(Vector3 v) {
    return (Vector3) {
        .x = roundf(v.x),
        .y = roundf(v.y),
        .z = roundf(v.z)
    };
}

Vector3 vector3_floor(Vector3 v) {
    return (Vector3) {
        .x = floorf(v.x),
        .y = floorf(v.y),
        .z = floorf(v.z)
    };
}

Vector3 vector3_ceil(Vector3 v) {
    return (Vector3) {
        .x = ceilf(v.x),
        .y = ceilf(v.y),
        .z = ceilf(v.z)
    };
}

Vector3 vector3_log(Vector3 v) {
    return (Vector3) {
        .x = logf(v.x),
        .y = logf(v.y),
        .z = logf(v.z)
    };
}

Vector3 vector3_exp(Vector3 v) {
    return (Vector3) {
        .x = expf(v.x),
        .y = expf(v.y),
        .z = expf(v.z)
    };
}
