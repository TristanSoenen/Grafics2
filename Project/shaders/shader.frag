#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D Diffuse;
layout(binding = 2) uniform sampler2D Normal;
layout(binding = 3) uniform sampler2D Gloss;
layout(binding = 4) uniform sampler2D Specular;

void main() 
{
    outColor = texture(Diffuse, fragTexCoord);
}