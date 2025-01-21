#pragma once
#include "vector3.h"

typedef struct Quaternion {
    float w, x, y, z;
} Quaternion;

static const Quaternion quaternion_identity = { .w = 1.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f };

float degrees(float radians);
float radians(float degrees);

Quaternion quaternion_add(Quaternion a, Quaternion b);
Quaternion quaternion_subtract(Quaternion a, Quaternion b);
Quaternion quaternion_multiply(Quaternion a, Quaternion b);
Quaternion quaternion_scale(Quaternion q, float s);
float quaternion_magnitude(Quaternion q);
Quaternion quaternion_normalize(Quaternion q);
Quaternion quaternion_conjugate(Quaternion q);
Quaternion quaternion_inverse(Quaternion q);
Quaternion quaternion_angle_axis(float angle, Vector3 axis);
Vector3 quaternion_rotate_vector(Quaternion q, Vector3 v);

float quaternion_extract_roll_angle(Quaternion q);
float quaternion_extract_yaw_angle(Quaternion q);
float quaternion_extract_pitch_angle(Quaternion q);

Quaternion quaternion_slerp(Quaternion a, Quaternion b, float t);