#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

const float zNear = 20.0;
const float zFar = 50.0;

#define M_PI 3.1415926535897932384626433832795

float getAmbientOcclusion(vec3 midpoint, vec3 normal, vec3 sample_point)
{
	vec3 diff = sample_point - midpoint;
	float diff_len_sqr = dot(diff, diff);
	float raw_power = dot(normal, diff) * 1.0 / sqrt(diff_len_sqr);

	return clamp(raw_power - 0.0, 0, 1) * clamp(diff_len_sqr * -4.0 + 1.0, 0, 1);
}

vec3 getPosition(vec2 co, float depth)
{
	return vec3((co - textureSize(texSampler, 0) * 0.5) / textureSize(texSampler, 0).y * vec2(2.0, -2.0) * 0.26794919243112270647255365849413 * depth, depth);
}

float linearize_depth(float d)
{
	return zNear * zFar / (zFar + d * (zNear - zFar));
}

vec3 getPosition(vec2 co)
{
	float depth = linearize_depth(texelFetch(texSampler, ivec2(co), 0).r);
	return getPosition(co, depth);
}

vec3 getClosest(vec3 midpoint, vec3 right, vec3 left)
{
	vec3 diff_right = right - midpoint;
	vec3 diff_left = midpoint - left;
	return dot(diff_right, diff_right) < dot(diff_left, diff_left) ? diff_right : diff_left;
}

vec3 getNormal(vec2 co, vec3 midpoint)
{
	vec3 right = getPosition(co + vec2(1, 0));
	vec3 left = getPosition(co + vec2(-1, 0));
	vec3 top = getPosition(co + vec2(0, 1));
	vec3 bottom = getPosition(co + vec2(0, -1));
	vec3 normal = normalize(cross(getClosest(midpoint, right, left), getClosest(midpoint, top, bottom)));
	return normal;
}

void main()
{
	vec2 co = gl_FragCoord.xy;
	vec3 midpoint = getPosition(co);
	vec3 normal = getNormal(co, midpoint);
	float ao = 0.0;

	float modifier = midpoint.x * 329003.3665;
	modifier = mod(modifier, M_PI * 2.0);
	modifier += midpoint.y * 248293.321;
	modifier = mod(modifier, M_PI * 2.0);
	modifier += midpoint.z * 223123.321;
	modifier = mod(modifier, M_PI * 2.0);

	float step_size = textureSize(texSampler, 0).y * (5.0 / 18.0) / midpoint.z;
	for (int rotation = 0; rotation < 4; ++rotation)
	{
		float angle = rotation * M_PI * 2.0 / 4;
		float distance = step_size * mod(modifier + rotation * 2.234, 1.0) + 1.0;
		for (int step = 0; step < 4; ++step)
		{
			vec2 sample_coord = co + round(vec2(cos(angle + modifier), sin(angle + modifier)) * distance);
			ao += getAmbientOcclusion(midpoint, normal, getPosition(sample_coord));
			distance += step_size;
		}
	}
	ao *= 1.5 / (4 * 4);
	ao = clamp(1.0 - ao * 2.0, 0, 1);
	ao = pow(ao, 1.0);
	
	//outColor = vec4(vec3(0.0), 1.0 - ao);
	outColor = vec4(vec3(ao), 1.0);
}
