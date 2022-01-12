#pragma once

#include "Resource.h"

#include <vector>
#include <map>
#include <array>

#include "Matrix4.h"

class Animation :
	public Resource<Animation>
{
public:
	Animation();
	~Animation();

	static std::shared_ptr<Animation> load(const std::string & fname);

	class Bone
	{
	public:
		Matrix4 matrix;
		uint64_t parent;
	};

	class Pose
	{
	public:
		std::vector<Bone> bones;

		void apply(Matrix4 * transforms) const;
	};

	class Channel
	{
	public:
		std::map<float, float> keyframes;

		float operator[](float time) const;
	};

	class Action
	{
	public:
		std::vector<std::array<Channel, 10>> channels;
		float length;

		void getPose(Pose& pose, Pose& base_pose, float time) const;
	};

	Pose base_pose;
	std::vector<Matrix4> base_pose_inverse_transforms;
	std::map<std::string, Action> actions;
};
