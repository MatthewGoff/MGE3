#version 450

layout(binding = 1) uniform sampler2D texSampler[10];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
flat layout(location = 2) in uint fragTexIndex;

layout(location = 0) out vec4 outColor;

void main()
{
    //outColor = vec4(fragColor, 1.0);
    //outColor = vec4(fragTexCoord, 0.0, 1.0);
    outColor = texture(texSampler[fragTexIndex], fragTexCoord);
}