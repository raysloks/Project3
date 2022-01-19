#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    float light = max(0.0, dot(normalize(fragNormal), normalize(vec3(1.0, -1.5, 1.0))));
    vec3 lightColor = mix(mix(vec3(0.3, 0.3, 0.3), vec3(0.2, 0.2, 0.5), fragNormal.z + 1.0), vec3(1.1, 1.1, 0.8), light);
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = vec4(fragColor.rgb * lightColor * texColor.rgb, fragColor.a * texColor.a);
}