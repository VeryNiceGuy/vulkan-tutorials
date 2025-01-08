#include "dual_quaternion.h"
#include "quaternion.h"
#include "vector3.h"

DualQuaternion dual_quaternion_create(Quaternion rotation, Vector3 translation) {
    DualQuaternion dq = {
        .real = rotation,
        .dual = quaternion_multiply_scalar(((Quaternion) {
            .w = 0.0f,
            .x = translation.x,
            .y = translation.y,
            .z = translation.z
        }, rotation), 0.5f)
    };
    return dq;
}

Vector3 dual_quaternion_get_translation(DualQuaternion dq) {
    Quaternion q = quaternion_multiply_scalar(quaternion_multiply(dq.dual, dq.real), 2.0f);
    return (Vector3) { .x = q.x, .y = q.y, .z = q.z };
}

DualQuaternion dual_quaternion_add(DualQuaternion a, DualQuaternion b) {
    return (DualQuaternion) {
        .real = quaternion_add(a.real, b.real),
        .dual = quaternion_add(a.dual, b.dual)
    };
}

DualQuaternion dual_quaternion_subtract(DualQuaternion a, DualQuaternion b) {
    return (DualQuaternion) {
        .real = quaternion_subtract(a.real, b.real),
        .dual = quaternion_subtract(a.dual, b.dual)
    };
}

DualQuaternion dual_quaternion_multiply(DualQuaternion a, DualQuaternion b) {
    return (DualQuaternion) {
        .real = quaternion_multiply(a.real, b.real),
        .dual = quaternion_add(quaternion_multiply(a.real, b.dual), quaternion_multiply(a.dual, b.real))
    };
}

DualQuaternion dual_quaternion_multiply_scalar(DualQuaternion dq, float scalar) {
    return (DualQuaternion) {
        .real = quaternion_multiply_scalar(dq.real, scalar),
        .dual = quaternion_multiply_scalar(dq.dual, scalar)
    };
}

DualQuaternion dual_quaternion_conjugate(DualQuaternion dq) {
    return (DualQuaternion) {
        .real = quaternion_conjugate(dq.real),
        .dual = quaternion_conjugate(dq.dual)
    };
}

DualQuaternion dual_quaternion_normalize(DualQuaternion dq) {
    float mag = quaternion_magnitude(dq.real);

    return (DualQuaternion) {
        .real = quaternion_normalize(dq.real),
        .dual = quaternion_multiply_scalar(dq.dual, 1.0f / mag)
    };
}
