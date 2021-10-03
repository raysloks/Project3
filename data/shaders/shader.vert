#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in ivec4 inBones;
layout(location = 4) in vec4 inWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 bones[256];
} ubo;

layout(binding = 1) uniform ViewProjection {
	mat4 view;
	mat4 proj;
} vp;

void main() {
	mat4 bone_transform = ubo.bones[inBones.x] * inWeights.x + 
		ubo.bones[inBones.y] * inWeights.y + 
		ubo.bones[inBones.z] * inWeights.z + 
		ubo.bones[inBones.w] * inWeights.w;
	bone_transform /= inWeights.x + inWeights.y + inWeights.z + inWeights.w;
	gl_Position = vp.proj * vp.view * ubo.model * bone_transform * vec4(inPosition, 1.0);
	fragNormal = (ubo.model * bone_transform * vec4(inNormal, 0.0)).xyz;
	fragColor = vec3(1.0);
	fragTexCoord = inUV;
}