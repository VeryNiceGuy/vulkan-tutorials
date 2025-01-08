#pragma once

#include "vector3.h"
#include "matrix4x4.h"
#include "dual_quaternion.h"

typedef struct Camera {
	DualQuaternion transform;
} Camera;

void camera_init_euler_angles(Camera* camera, float yaw, float pitch, float roll, Vector3 position);
void camera_init_quaternion(Camera* camera, Quaternion rotation, Vector3 position);
void camera_init_dual_quaternion(Camera* camera, DualQuaternion transform);

void camera_yaw(Camera* camera, float angle);
void camera_pitch(Camera* camera, float angle);
void camera_roll(Camera* camera, float angle);

Quaternion camera_get_rotation(Camera* camera);
void camera_set_rotation(Camera* camera, Quaternion rotation);

Vector3 camera_get_position(Camera* camera);
void camera_set_position(Camera* camera, Vector3 position);

void camera_rotate(Camera* camera, Quaternion rotation);
void camera_translate(Camera* camera, Vector3 translation);

Vector3 camera_get_direction(Camera* camera);
void camera_move(Camera* camera, float step);

Matrix4x4 camera_calculate_view_matrix(Camera* camera);