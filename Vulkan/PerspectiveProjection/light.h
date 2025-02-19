#pragma once
#include "vector3.h"

typedef enum LightType {
	POINT,
	SPOT
} LightType;

typedef struct RgbColor {
	float r, g, b;
} RgbColor;

typedef struct Light {
	LightType lightType;
	RgbColor color;
	float intensity;
	float range;
	float innerConeAngle;
	float outerConeAngle;
} Light;

typedef struct AmbientLight {
	float color[3];
	float intensity;
} AmbientLight;

typedef struct {
	Vector3 direction;
	RgbColor color;
	float intensity;
} DirectionalLight;