#include "camera.h"

void camera_init_euler_angles(Camera* camera, float yaw, float pitch, float roll, Vector3 position) {
    camera->rotation = quaternion_multiply(
        quaternion_angle_axis(yaw, vector3_unit_y),
        quaternion_multiply(quaternion_angle_axis(pitch, vector3_unit_x), quaternion_angle_axis(roll, vector3_unit_z)));
    camera->position = position;
}

void camera_init_quaternion(Camera* camera, Quaternion rotation, Vector3 position) {
    camera->rotation = rotation;
    camera->position = position;
}

void camera_yaw(Camera* camera, float angle) {
    camera->rotation = quaternion_normalize(
        quaternion_multiply(quaternion_angle_axis(angle,
            vector3_normalize(vector3_unit_y)), camera->rotation));
}

void camera_pitch(Camera* camera, float angle) {
    camera->rotation = quaternion_normalize(
        quaternion_multiply(camera->rotation,
            quaternion_angle_axis(angle,
                vector3_normalize(vector3_unit_x))));
}

/*
void camera_yaw(Camera* camera, float angle) {
    float t = 0.5f;
    Quaternion yaw = quaternion_angle_axis(angle, (Vector3) { 0.0f, 1.0f, 0.0f });
    camera->rotation = quaternion_slerp(camera->rotation, quaternion_multiply(camera->rotation, yaw), t);
}

void camera_pitch(Camera* camera, float angle) {
    float t = 0.5f;
    Quaternion pitch = quaternion_angle_axis(angle, (Vector3) { 1.0f, 0.0f, 0.0f });
    camera->rotation = quaternion_slerp(camera->rotation, quaternion_multiply(camera->rotation, pitch), t);
}*/


void camera_roll(Camera* camera, float angle) {
    camera->rotation = quaternion_normalize(
        quaternion_multiply(camera->rotation,
            quaternion_angle_axis(angle, vector3_unit_z)));
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
    camera_translate(camera, vector3_scale(camera_get_direction(camera), step));
}

Matrix4x4 camera_calculate_view_matrix(Camera* camera) {
    return matrix4x4_from_quaternion_vector3(camera->rotation, camera->position);
};
