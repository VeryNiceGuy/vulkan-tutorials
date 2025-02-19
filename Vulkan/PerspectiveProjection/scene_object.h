#pragma once
#include "vector3.h"
#include "quaternion.h"
#include "light.h"
#include "mesh.h"

typedef enum SceneObjectType {
	CAMERA,
	LIGHT,
	MESH
} SceneObjectType;

typedef struct SceneObject {
	SceneObjectType type;
	Vector3 position;
	Quaternion orientation;
	Mesh* mesh;
	Light* light;
} SceneObject;
