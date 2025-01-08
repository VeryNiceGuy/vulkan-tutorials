#include "camera.h"

void camera_yaw(Camera* camera, float angle) {
    Quaternion yaw = quaternion_angle_axis(angle, (Vector3) { 0.0f, 1.0f, 0.0f });
    Vector3 translation = dual_quaternion_get_translation(camera->transform);
    camera->transform = dual_quaternion_create(quaternion_multiply(yaw, camera->transform.real), translation);
}

void camera_pitch(Camera* camera, float angle) {
    Quaternion pitch = quaternion_angle_axis(angle, (Vector3) { 1.0f, 0.0f, 0.0f });
    Vector3 translation = dual_quaternion_get_translation(camera->transform);
    camera->transform = dual_quaternion_create(quaternion_multiply(pitch, camera->transform.real), translation);
}

void camera_roll(Camera* camera, float angle) {
    Quaternion roll = quaternion_angle_axis(angle, (Vector3) { 0.0f, 0.0f, 1.0f });
    Vector3 translation = dual_quaternion_get_translation(camera->transform);
    camera->transform = dual_quaternion_create(quaternion_multiply(roll, camera->transform.real), translation);
}

Quaternion camera_get_rotation(Camera* camera) {
	return camera->transform.real;
}

void camera_set_rotation(Camera* camera, Quaternion rotation) {
	Vector3 translation = dual_quaternion_get_translation(camera->transform);
	camera->transform = dual_quaternion_create(rotation, translation);
}

Vector3 camera_get_position(Camera* camera) {
	return dual_quaternion_get_translation(camera->transform);
}

void camera_set_position(Camera* camera, Vector3 position) {
	Vector3 translation = vector3_subtract(position, dual_quaternion_get_translation(camera->transform));
	camera->transform = dual_quaternion_create(camera->transform.real, translation);
}

void camera_rotate(Camera* camera, Quaternion rotation) {
    Vector3 translation = dual_quaternion_get_translation(camera->transform);
    camera->transform = dual_quaternion_create(quaternion_multiply(rotation, camera->transform.real), translation);
}

void camera_translate(Camera* camera, Vector3 translation) {
    camera->transform = dual_quaternion_create(camera->transform.real, translation);
}

Matrix4x4 camera_calculate_view_matrix(Camera* camera) {
    return matrix4x4_from_dual_quaternion(camera->transform);
};

/*Matrix4x4 camera_calculate_view_matrix(Camera* camera) {
    return (Matrix4x4) {
        .m_11 = camera->right.x, .m_12 = camera->up.x, .m_13 = camera->look.x, .m_14 = 0.0f,
        .m_21 = camera->right.y, .m_22 = camera->up.y, .m_23 = camera->look.y, .m_24 = 0.0f,
        .m_31 = camera->right.z, .m_32 = camera->up.z, .m_33 = camera->look.z, .m_34 = 0.0f,

        .m_41 = -vector3_dot(camera->right, camera->position),
        .m_42 = -vector3_dot(camera->up, camera->position),
        .m_43 = -vector3_dot(camera->look, camera->position),
        .m_44 = 1.0f
    };
};*/
