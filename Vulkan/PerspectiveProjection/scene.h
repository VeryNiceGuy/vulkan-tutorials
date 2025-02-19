#pragma once
#include "light.h"
#include "scene_object.h"

typedef struct Scene {
	AmbientLight* ambientLight;
	DirectionalLight* directionalLight;
	size_t sceneObjectCount;
	SceneObject* sceneObjects;
} Scene;
