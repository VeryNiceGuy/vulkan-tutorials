#version 450

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_MATERIALS 10

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    float intensity;
    vec3 color;
    float range;
};

struct DirectionalLight {
    vec3 direction;
    float intensity;
    vec3 color;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float intensity;
    vec3 color;
    float range;
    float innerConeAngle;
    float outerConeAngle;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float glossiness;
    float opacity;
    float transparency;
    uint illumination_model;
    bool hasDiffuseMap;
    bool hasNormalMap;
    bool hasEmissiveMap;
    bool hasAlphaMap;
    bool hasBumpMap;
    bool hasDisplacementMap;
};

layout(location = 0) in vec2 frag_uv_coords;
layout(binding = 1) uniform sampler2D texture_sampler;

layout(binding = 2) uniform MaterialsUB {
    Material materials[MAX_MATERIALS];
} materialsi;

layout(binding = 3) uniform sampler2D diffuseMaps[MAX_MATERIALS];
layout(binding = 4) uniform sampler2D normalMaps[MAX_MATERIALS];
layout(binding = 5) uniform sampler2D specularMaps[MAX_MATERIALS];
layout(binding = 6) uniform sampler2D emissiveMaps[MAX_MATERIALS];
layout(binding = 7) uniform sampler2D alphaMaps[MAX_MATERIALS];
layout(binding = 8) uniform sampler2D bumpMaps[MAX_MATERIALS];
layout(binding = 9) uniform sampler2D displacementMaps[MAX_MATERIALS];

layout(location = 0) out vec4 out_color;

void main() {
    vec4 diffuseColor = vec4(1.0);
    vec4 normalColor = vec4(0.5, 0.5, 1.0, 1.0);
    vec4 specularColor = vec4(1.0);
    vec4 emissiveColor = vec4(0.0);
    vec4 alphaColor = vec4(1.0);
    vec4 bumpColor = vec4(0.5, 0.5, 1.0, 1.0);
    vec4 displacementColor = vec4(0.0);
	
    out_color = texture(texture_sampler, frag_uv_coords);
}
