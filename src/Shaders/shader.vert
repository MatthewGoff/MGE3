#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    vec2 camPosition;
    vec2 camDimensions;
} ubo;

void main()
{
    vec2 final_position = inPosition - ubo.camPosition;
    
    gl_Position = vec4(final_position, 0.0, 1.0);
    
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}