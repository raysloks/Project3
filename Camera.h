#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "Quaternion.h"

class Camera
{
public:
	// degrees (0, 180)
	float field_of_view;
	float near_clip, far_clip;

	Vec3 position;
	Quaternion rotation;

	Vec2 shift;
	float aspect_ratio_multiplier;
};
