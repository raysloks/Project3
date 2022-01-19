#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    float dist = ( texColor.a - 0.5 ) * 2.0;
    float pixel_size = min(2.0, fwidth(fragTexCoord.x) * 0.5 * 4096.0 * 0.5);
    float coverage = smoothstep(-pixel_size * 0.5, pixel_size * 0.5, dist);
    outColor = vec4(fragColor.rgb * texColor.rgb, fragColor.a * coverage);
}