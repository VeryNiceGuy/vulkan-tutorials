#pragma once
#include "vector3.h"
#include "quaternion.h"
#include "dual_quaternion.h"

typedef struct Matrix4x4 {
	float m_11, m_12, m_13, m_14;
	float m_21, m_22, m_23, m_24;
	float m_31, m_32, m_33, m_34;
	float m_41, m_42, m_43, m_44;
} Matrix4x4;

Matrix4x4 matrix4x4_create_identity();
Matrix4x4 matrix4x4_create_look_at_lh(Vector3 eye, Vector3 at, Vector3 up);
Matrix4x4 matrix4x4_create_look_at_rh(Vector3 eye, Vector3 at, Vector3 up);
Matrix4x4 matrix4x4_perspective_fov_lh_d3d(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 matrix4x4_perspective_fov_rh_d3d(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 matrix4x4_perspective_fov_lh_gl(float fovY, float aspect, float nearZ, float farZ);
Matrix4x4 matrix4x4_perspective_fov_rh_gl(float fovY, float aspect, float nearZ, float farZ);
float matrix4x4_determinant(Matrix4x4 m);
Matrix4x4 matrix4x4_adjugate(Matrix4x4 m);
Matrix4x4 matrix4x4_inverse(Matrix4x4 m, int* success);
Matrix4x4 matrix4x4_transpose(Matrix4x4 m);
Matrix4x4 matrix4x4_add(Matrix4x4 a, Matrix4x4 b);
Matrix4x4 matrix4x4_subtract(Matrix4x4 a, Matrix4x4 b);
Matrix4x4 matrix4x4_multiply(Matrix4x4 a, Matrix4x4 b);
Matrix4x4 matrix4x4_scale(Matrix4x4 m, float s);
Matrix4x4 matrix4x4_from_quaternion(Quaternion q);
Matrix4x4 matrix4x4_from_quaternion_vector3(Quaternion q, Vector3 v);
Matrix4x4 matrix4x4_from_dual_quaternion(DualQuaternion dq);