#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in flat int fragmapIndex;

layout(location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;


layout(binding = 1) uniform sampler2D Diffuse;
layout(binding = 2) uniform sampler2D Normal;
layout(binding = 3) uniform sampler2D Gloss;
layout(binding = 4) uniform sampler2D Specular;

void main() 
{
    //lights



    vec3 diff = texture(Diffuse, fragTexCoord).rgb; 


    vec3 color;
    //outColor = vec4(diff, 1.0); //texture(Diffuse, fragTexCoord);
    //outNormal = texure(Normal)

    if(fragmapIndex == 0)
    {
        //Combined
        color = diff;
    }
    else if(fragmapIndex == 1)
    {
        vec3 red = vec3(1, 0, 0);
        //Normal
        color = red;
    }
    else if(fragmapIndex == 2)
    {
        vec3 green = vec3(0, 1, 0);
        //Metalness
        color = green;
    }
    else if(fragmapIndex == 3)
    {
        vec3 blue = vec3(0, 0, 1);
        //Specular
        color = blue;
    }
    outColor = vec4(color, 1.0);

}