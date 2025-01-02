#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform ViewProjection {
	mat4 view;
	mat4 proj;
	float time;
} vp;

layout(binding = 2) uniform sampler2D depthSampler;
layout(binding = 3) uniform sampler2D normalSampler;

layout(location = 0) out vec4 outColor;

const float zNear = 20.0;
const float zFar = 80.0;

#define M_PI 3.1415926535897932384626433832795

float rand(vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float linearize_depth(float d)
{
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

float getDepth(vec2 co)
{
	return linearize_depth(texture(depthSampler, co / textureSize(depthSampler, 0)).r);
}

vec3 getNormal(vec2 co)
{
	return texture(normalSampler, co / textureSize(normalSampler, 0)).xyz;
}

float sobelDepth(float stepx, float stepy, vec2 center)
{
	float nw = getDepth(center + vec2(-stepx, stepy));
	float w  = getDepth(center + vec2(-stepx, 0));
	float sw = getDepth(center + vec2(-stepx, -stepy));
	float n  = getDepth(center + vec2(0, stepy));
	float s  = getDepth(center + vec2(0, -stepy));
	float ne = getDepth(center + vec2(stepx, stepy));
	float e  = getDepth(center + vec2(stepx, 0));
	float se = getDepth(center + vec2(stepx, -stepy));
 
	// Sobel masks (see http://en.wikipedia.org/wiki/Sobel_operator)
	//        1 0 -1     -1 -2 -1
	//    X = 2 0 -2  Y = 0  0  0
	//        1 0 -1      1  2  1
	
	// You could also use Scharr operator:
	//        3 0 -3        3 10   3
	//    X = 10 0 -10  Y = 0  0   0
	//        3 0 -3        -3 -10 -3
 
	float x = nw + 2.0 * w + sw - ne - 2.0 * e - se;
	float y = -nw - 2.0 * n - ne + sw + 2.0 * s + se;
	float value = sqrt((x * x) + (y * y));
	return step(0.1, value);
 }

float sobelNormal(float stepx, float stepy, vec2 center)
{
	vec3 nw = getNormal(center + vec2(-stepx, stepy));
	vec3 w  = getNormal(center + vec2(-stepx, 0));
	vec3 sw = getNormal(center + vec2(-stepx, -stepy));
	vec3 n  = getNormal(center + vec2(0, stepy));
	vec3 s  = getNormal(center + vec2(0, -stepy));
	vec3 ne = getNormal(center + vec2(stepx, stepy));
	vec3 e  = getNormal(center + vec2(stepx, 0));
	vec3 se = getNormal(center + vec2(stepx, -stepy));
 
	// Sobel masks (see http://en.wikipedia.org/wiki/Sobel_operator)
	//        1 0 -1     -1 -2 -1
	//    X = 2 0 -2  Y = 0  0  0
	//        1 0 -1      1  2  1
	
	// You could also use Scharr operator:
	//        3 0 -3        3 10   3
	//    X = 10 0 -10  Y = 0  0   0
	//        3 0 -3        -3 -10 -3
 
	vec3 x = nw + w * 2.0 + sw - ne - e * 2.0 - se;
	vec3 y = -nw - n * 2.0 - ne + sw + s * 2.0 + se;
	float value = sqrt(dot(x, x) + dot(y, y));
	return step(1.0, value);
 }

void main()
{
	vec2 co = gl_FragCoord.xy;
	//co += vec2(rand(co + vp.time), rand(co - vp.time)) * 0.5 - 0.25;
	//float step_size = rand(co + vp.time) * 0.25 + 0.25;
	float step_size = 0.25;
	float sss = 0.5;
	float value = 0.0;
	value += sobelNormal(step_size, step_size, co + vec2(-sss, -sss));
	value += sobelNormal(step_size, step_size, co + vec2(0, -sss));
	value += sobelNormal(step_size, step_size, co + vec2(sss, -sss));
	value += sobelNormal(step_size, step_size, co + vec2(-sss, 0));
	value += sobelNormal(step_size, step_size, co + vec2(0, 0));
	value += sobelNormal(step_size, step_size, co + vec2(sss, 0));
	value += sobelNormal(step_size, step_size, co + vec2(-sss, sss));
	value += sobelNormal(step_size, step_size, co + vec2(0, sss));
	value += sobelNormal(step_size, step_size, co + vec2(sss, sss));
	float result = smoothstep(0.0, 0.7, value / 10.0);

	step_size = 0.25;
	sss = 0.5;
	value = 0.0;
	value += sobelDepth(step_size, step_size, co + vec2(-sss, -sss));
	value += sobelDepth(step_size, step_size, co + vec2(0, -sss));
	value += sobelDepth(step_size, step_size, co + vec2(sss, -sss));
	value += sobelDepth(step_size, step_size, co + vec2(-sss, 0));
	value += sobelDepth(step_size, step_size, co + vec2(0, 0));
	value += sobelDepth(step_size, step_size, co + vec2(sss, 0));
	value += sobelDepth(step_size, step_size, co + vec2(-sss, sss));
	value += sobelDepth(step_size, step_size, co + vec2(0, sss));
	value += sobelDepth(step_size, step_size, co + vec2(sss, sss));
	result = max(result, smoothstep(0.0, 0.7, value / 10.0));
	outColor = vec4(vec3(0.0), result);
	//outColor = vec4(vec3(0.0), max(sobelNormal(step_size, step_size, co), sobelDepth(step_size, step_size, co)));
}
