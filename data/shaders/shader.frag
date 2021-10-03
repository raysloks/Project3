#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    float light = step(0.35, dot(normalize(fragNormal), normalize(vec3(1.0, 0.0, 1.0)))) * 0.5 + 0.5;
    outColor = vec4(sqrt(fragColor * light * texture(texSampler, fragTexCoord).rgb), 1.0);
}