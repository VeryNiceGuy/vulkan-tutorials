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

Vector3 vector3_add(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z
    };
}

Vector3 vector3_add_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x + s,
        .y = v.y + s,
        .z = v.z + s
    };
}

Vector3 vector3_subtract(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z
    };
}

Vector3 vector3_subtract_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x - s,
        .y = v.y - s,
        .z = v.z - s
    };
}

Vector3 vector3_multiply(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = v1.x * v2.x,
        .y = v1.y * v2.y,
        .z = v1.z * v2.z
    };
}

Vector3 vector3_multiply_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
}

Vector3 vector3_divide(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = v1.x / v2.x,
        .y = v1.y / v2.y,
        .z = v1.z / v2.z
    };
}

Vector3 vector3_divide_scalar(Vector3 v, float s) {
    return (Vector3) {
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
}

Vector3 vector3_cross(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = v1.y * v2.z - v1.z * v2.y,
        .y = v1.z * v2.x - v1.x * v2.z,
        .z = v1.x * v2.y - v1.y * v2.x
    };
}

float vector3_dot(Vector3 v1, Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float vector3_distance_to(Vector3 v1, Vector3 v2) {
    return sqrtf(
        (v1.x - v2.x) * (v1.x - v2.x)
        + (v1.y - v2.y) * (v1.y - v2.y)
        + (v1.z - v2.z) * (v1.z - v2.z));
}

float vector3_angle_between(Vector3 v1, Vector3 v2) {
    float dotProduct = vector3_dot(v1, v2);
    float magnitudes = vector3_magnitude(v1) * vector3_magnitude(v2);

    return acosf(dotProduct / magnitudes);
}

float vector3_angle_to(Vector3 v1, Vector3 v2) {
    return atan2f(vector3_magnitude(vector3_cross(v1, v2)), vector3_dot(v1, v2));
}

Vector3 vector3_reflect(Vector3 v, Vector3 normal) {
    return vector3_subtract(v, vector3_multiply_scalar(normal, 2 * vector3_dot(v, normal)));
}

Vector3 vector3_lerp(Vector3 v1, Vector3 v2, float t) {
    return vector3_add(vector3_multiply_scalar(v1, 1.0f - t), vector3_multiply_scalar(v2, t));
}

Vector3 vector3_clamp(Vector3 v, Vector3 min, Vector3 max) {
    return (Vector3) {
        .x = fmaxf(min.x, fminf(v.x, max.x)),
        .y = fmaxf(min.y, fminf(v.y, max.y)),
        .z = fmaxf(min.z, fminf(v.z, max.z))
    };
}

Vector3 vector3_midpoint(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = (v1.x + v2.x) / 2.0f,
        .y = (v1.y + v2.y) / 2.0f,
        .z = (v1.z + v2.z) / 2.0f
    };
}

Vector3 vector3_project_onto(Vector3 v1, Vector3 v2) {
    float dotProduct = vector3_dot(v1, v2);
    float otherMagnitudeSquared = vector3_magnitude(v2) * vector3_magnitude(v2);

    return vector3_multiply_scalar(v2, dotProduct / otherMagnitudeSquared);
}

Vector3 vector3_perpendicular_to(Vector3 v1, Vector3 v2) {
    return vector3_subtract(v1, vector3_project_onto(v1, v2));
}

float vector3_triple_product(Vector3 v, Vector3 v1, Vector3 v2) {
    return vector3_dot(v, vector3_cross(v1, v2));
}

Vector3 vector3_rotate_around_axis(Vector3 v, Vector3 axis, float angle) {
    Vector3 normalizedAxis = vector3_normalize(axis);
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return vector3_add(
        vector3_add(vector3_multiply_scalar(v, cosAngle), vector3_multiply_scalar(vector3_cross(normalizedAxis, v), sinAngle)),
        vector3_multiply_scalar(normalizedAxis, vector3_dot(normalizedAxis, v) * (1.0f - cosAngle))
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

Vector3 vector3_min(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = fminf(v1.x, v2.x),
        .y = fminf(v1.y, v2.y),
        .z = fminf(v1.z, v2.z)
    };
}

Vector3 vector3_max(Vector3 v1, Vector3 v2) {
    return (Vector3) {
        .x = fmaxf(v1.x, v2.x),
        .y = fmaxf(v1.y, v2.y),
        .z = fmaxf(v1.z, v2.z)
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
