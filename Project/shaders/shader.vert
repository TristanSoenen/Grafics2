#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 invView;
    mat4 proj;
    int mapindex;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragTangent;
layout(location = 4) out int fragmapIndex;
layout(location = 5) out mat4 fraginvView;
layout(location = 10) out vec3 fragWorldPos;

void main() 
{
    vec4 worldPosition = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
    fragTangent = inTangent;
    fragmapIndex = ubo.mapindex;
    fraginvView = ubo.invView;
    fragWorldPos = worldPosition.xyz;
}