#include "camera.h"

void camera_init_euler_angles(Camera* camera, float yaw, float pitch, float roll, Vector3 position) {
    Quaternion yaw_quaternion = quaternion_angle_axis(yaw, vector3_unit_y);
    Quaternion pitch_quaternion = quaternion_angle_axis(pitch, vector3_unit_x);
    Quaternion roll_quaternion = quaternion_angle_axis(roll, vector3_unit_z);

    camera->rotation = quaternion_multiply(yaw_quaternion, quaternion_multiply(pitch_quaternion, roll_quaternion));
    camera->position = position;
}

void camera_init_quaternion(Camera* camera, Quaternion rotation, Vector3 position) {
    camera->rotation = rotation;
    camera->position = position;
}

void camera_yaw(Camera* camera, float angle) {
    Quaternion yaw = quaternion_angle_axis(angle, vector3_unit_y);
    camera->rotation = quaternion_multiply(yaw, camera->rotation);
}

void camera_pitch(Camera* camera, float angle) {
    Quaternion pitch = quaternion_angle_axis(angle, vector3_unit_x);
    camera->rotation = quaternion_multiply(pitch, camera->rotation);
}

void camera_roll(Camera* camera, float angle) {
    Quaternion roll = quaternion_angle_axis(angle, vector3_unit_z);
    camera->rotation = quaternion_multiply(roll, camera->rotation);
}

Quaternion camera_get_rotation(Camera* camera) {
	return camera->rotation;
}

void camera_set_rotation(Camera* camera, Quaternion rotation) {
    camera->rotation = rotation;
}

Vector3 camera_get_position(Camera* camera) {
	return camera->position;
}

void camera_set_position(Camera* camera, Vector3 position) {
    camera->position = position;
}

void camera_rotate(Camera* camera, Quaternion rotation) {
    camera->rotation = quaternion_multiply(rotation, camera->rotation);
}

void camera_translate(Camera* camera, Vector3 translation) {
    camera->position = vector3_add(camera->position, translation);
}

Vector3 camera_get_direction(Camera* camera) {
    return vector3_normalize(quaternion_rotate_vector(camera->rotation, vector3_unit_z));
}

void camera_move(Camera* camera, float step) {
    camera_translate(camera, vector3_multiply_scalar(camera_get_direction(camera), step));
}

Matrix4x4 camera_calculate_view_matrix(Camera* camera) {
    return matrix4x4_from_quaternion_vector3(camera->rotation, camera->position);
};
