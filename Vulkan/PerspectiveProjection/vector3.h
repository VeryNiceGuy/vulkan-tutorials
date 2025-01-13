#pragma once

typedef struct Vector3 {
    float x, y, z;
} Vector3;

static const Vector3 vector3_unit_x = { .x = 1.0f, .y = 0.0f, .z = 0.0f };
static const Vector3 vector3_unit_y = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
static const Vector3 vector3_unit_z = { .x = 0.0f, .y = 0.0f, .z = 1.0f };

Vector3 vector3_normalize(Vector3 v);
float vector3_magnitude(Vector3 v);
Vector3 vector3_add(Vector3 v1, Vector3 v2);
Vector3 vector3_add_scalar(Vector3 v, float s);
Vector3 vector3_subtract(Vector3 v1, Vector3 v2);
Vector3 vector3_subtract_scalar(Vector3 v, float s);
Vector3 vector3_multiply(Vector3 v1, Vector3 v2);
Vector3 vector3_scale(Vector3 v, float s);
Vector3 vector3_divide(Vector3 v1, Vector3 v2);
Vector3 vector3_divide_scalar(Vector3 v, float s);
Vector3 vector3_cross(Vector3 v1, Vector3 v2);
float vector3_dot(Vector3 v1, Vector3 v2);
float vector3_distance_to(Vector3 v1, Vector3 v2);
float vector3_angle_between(Vector3 v1, Vector3 v2);
float vector3_angle_to(Vector3 v1, Vector3 v2);
Vector3 vector3_reflect(Vector3 v, Vector3 normal);
Vector3 vector3_lerp(Vector3 v1, Vector3 v2, float t);
Vector3 vector3_clamp(Vector3 v, Vector3 min, Vector3 max);
Vector3 vector3_midpoint(Vector3 v1, Vector3 v2);
Vector3 vector3_project_onto(Vector3 v1, Vector3 v2);
Vector3 vector3_perpendicular_to(Vector3 v1, Vector3 v2);
float vector3_triple_product(Vector3 v, Vector3 v1, Vector3 v2);
Vector3 vector3_rotate_around_axis(Vector3 v, Vector3 axis, float angle);
Vector3 vector3_rotate_x(Vector3 v, float angle);
Vector3 vector3_rotate_y(Vector3 v, float angle);
Vector3 vector3_rotate_z(Vector3 v, float angle);
Vector3 vector3_negate(Vector3 v);
Vector3 vector3_abs(Vector3 v);
float vector3_max_component(Vector3 v);
float vector3_min_component(Vector3 v);
Vector3 vector3_pow(Vector3 v, float exponent);
Vector3 vector3_sqrt(Vector3 v);
Vector3 vector3_min(Vector3 v1, Vector3 v2);
Vector3 vector3_max(Vector3 v1, Vector3 v2);
Vector3 vector3_round(Vector3 v);
Vector3 vector3_floor(Vector3 v);
Vector3 vector3_ceil(Vector3 v);
Vector3 vector3_log(Vector3 v);
Vector3 vector3_exp(Vector3 v);
