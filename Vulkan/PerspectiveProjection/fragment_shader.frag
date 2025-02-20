#version 450
layout(location = 0) in vec2 frag_uv_coords;
layout(binding = 1) uniform sampler2D texture_sampler;

layout(binding = 2) uniform sampler2D diffuseMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D specularMap;
layout(binding = 5) uniform sampler2D emissiveMap;
layout(binding = 6) uniform sampler2D alphaMap;
layout(binding = 7) uniform sampler2D bumpMap;
layout(binding = 8) uniform sampler2D displacementMap;

layout(location = 0) out vec4 out_color;

void main() {
    vec4 diffuseColor = vec4(1.0);
    vec4 normalColor = vec4(0.5, 0.5, 1.0, 1.0);
    vec4 specularColor = vec4(1.0);
    vec4 emissiveColor = vec4(0.0);
    vec4 alphaColor = vec4(1.0);
    vec4 bumpColor = vec4(0.5, 0.5, 1.0, 1.0);
    vec4 displacementColor = vec4(0.0);
	
    if (textureSize(diffuseMap, 0).x > 0) {
        diffuseColor = texture(diffuseMap, frag_uv_coords);
    }
    if (textureSize(normalMap, 0).x > 0) {
        normalColor = texture(normalMap, frag_uv_coords);
    }
    if (textureSize(specularMap, 0).x > 0) {
        specularColor = texture(specularMap, frag_uv_coords);
    }
    if (textureSize(emissiveMap, 0).x > 0) {
        emissiveColor = texture(emissiveMap, frag_uv_coords);
    }
    if (textureSize(alphaMap, 0).x > 0) {
        alphaColor = texture(alphaMap, frag_uv_coords);
    }
    if (textureSize(bumpMap, 0).x > 0) {
        bumpColor = texture(bumpMap, frag_uv_coords);
    }
    if (textureSize(displacementMap, 0).x > 0) {
        displacementColor = texture(displacementMap, frag_uv_coords);
    }
	
    out_color = texture(texture_sampler, frag_uv_coords);
}
