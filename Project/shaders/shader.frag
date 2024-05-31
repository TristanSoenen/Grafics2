#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragTangent;
layout(location = 4) in flat int fragmapIndex;
layout(location = 5) in mat4 fraginvView;
layout(location = 10) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;
layout (location = 1) out vec3 outNormal;

layout(binding = 1) uniform sampler2D Diffuse;
layout(binding = 2) uniform sampler2D Normal;
layout(binding = 3) uniform sampler2D Gloss;
layout(binding = 4) uniform sampler2D Specular;

const float PI = 3.14159f;
//functions

vec3 LambertDiffuse(float kd, const vec3 color)
{
    vec3 rho = color * kd;
    const vec3 lambertDiffuse = rho / PI ;
    return lambertDiffuse;
}

vec3 Phong(vec3 color, float exp, vec3 l, vec3 v, vec3 n)
{
    vec3 reflectvar = reflect(-l, n);
    float cosA = dot(reflectvar, v);
    if(cosA < 0.0f)
    {
        cosA = 0.0f;
    }
    const float extraShine = 25.0f;

    vec3 phongSpecularReflection = color * pow(cosA, exp * extraShine);
    return phongSpecularReflection;
}

void main() 
{
    //lights
    const vec3 lightDirection = vec3( 0.577, 0.577, 0.577 );
    const float lightIntensity = 7.0f;

    //normal
    vec3 binormal = cross( fragNormal, fragTangent );
    vec3 sampledNormal = texture(Normal, fragTexCoord).rgb;
    vec3 colorNormal = sampledNormal;
    sampledNormal = 2.0 * sampledNormal - vec3(1.0, 1.0, 1.0);

    float observedArea = dot( sampledNormal, -lightDirection );
    if(observedArea < 0.0f)
    {
        observedArea = 0.0f;
    }

    float shininess = texture(Gloss, fragTexCoord).r;
    vec3 specularReflectionCoeficient = texture(Specular, fragTexCoord).rgb;
    vec3 dir = normalize(fraginvView[3].xyz - fragWorldPos);

    vec3 diff = texture(Diffuse, fragTexCoord).rgb; 
    vec3 lambertDiffuse = LambertDiffuse(lightIntensity, diff);
    vec3 lambertPhong = Phong(specularReflectionCoeficient, shininess, lightDirection, dir, sampledNormal);
    vec3 ambient = vec3( 0.03, 0.03, 0.03 );

    vec3 color;
    if(fragmapIndex == 0)
    {
        //Diffuse
        color = observedArea * ( lambertDiffuse + ambient ) ;
    }
    else if(fragmapIndex == 1)
    {
        //Normal
        color = sampledNormal;
    }
    else if(fragmapIndex == 2)
    {
        //Specular
        color = observedArea * ( lambertPhong + ambient );
    }
    else if(fragmapIndex == 3)
    {
        //combined
        color = observedArea * (( lambertDiffuse + lambertPhong ) + ambient );
    }
    outColor = vec4(color, 1.0);

}