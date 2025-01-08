#pragma once
#include "quaternion.h"
#include "vector3.h"

typedef struct DualQuaternion {
    Quaternion real;
    Quaternion dual;
} DualQuaternion;

DualQuaternion dual_quaternion_create(Quaternion rotation, Vector3 translation);
Vector3 dual_quaternion_get_translation(DualQuaternion dq);
DualQuaternion dual_quaternion_add(DualQuaternion a, DualQuaternion b);
DualQuaternion dual_quaternion_subtract(DualQuaternion a, DualQuaternion b);
DualQuaternion dual_quaternion_multiply(DualQuaternion a, DualQuaternion b);
DualQuaternion dual_quaternion_multiply_scalar(DualQuaternion dq, float scalar);
DualQuaternion dual_quaternion_conjugate(DualQuaternion dq);
DualQuaternion dual_quaternion_normalize(DualQuaternion dq);
