#pragma once
#include "vector3.h"

typedef struct Quaternion {
    float w, x, y, z;
} Quaternion;

Quaternion quaternion_add(Quaternion a, Quaternion b);
Quaternion quaternion_subtract(Quaternion a, Quaternion b);
Quaternion quaternion_multiply(Quaternion a, Quaternion b);
Quaternion quaternion_multiply_scalar(Quaternion q, float s);
float quaternion_magnitude(Quaternion q);
Quaternion quaternion_normalize(Quaternion q);
Quaternion quaternion_conjugate(Quaternion q);
Quaternion quaternion_inverse(Quaternion q);
Quaternion quaternion_angle_axis(float angle, Vector3 axis);