#include "Animation.h"

#include <thread>
#include <fstream>
#include <sstream>

#include "Vec3.h"
#include "Quaternion.h"
#include "Matrix3.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

std::shared_ptr<Animation> Animation::load(const std::string & fname)
{
	auto animation = std::make_shared<Animation>();
	std::thread t([animation, fname]()
		{
			std::ifstream in(BaseResource::data_location + fname, std::ios::in | std::ios::binary);

			if (in)
			{
				std::string buffer;
				in.seekg(0, std::ios::end);
				buffer.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&buffer[0], buffer.size());
				in.close();

				std::stringstream ss(buffer);

				uint8_t flags = 0;
				ss.read((char*)&flags, sizeof(uint8_t));

				size_t bone_count = 0;
				ss.read((char*)&bone_count, sizeof(uint8_t));
				animation->base_pose.bones.resize(bone_count);

				char dummy[6];
				ss.read(dummy, sizeof(dummy));

				ss.read((char*)animation->base_pose.bones.data(), bone_count * sizeof(Bone));


				Matrix4 transforms[256];
				transforms[255] = Matrix4();
				animation->base_pose.apply(transforms);
				animation->base_pose_inverse_transforms.resize(bone_count);
				for (size_t i = 0; i < bone_count; ++i)
				{
					animation->base_pose_inverse_transforms[i] = transforms[i].Inverse();
				}


				float default_channel_values[10] = {
					0.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f,
					1.0f, 1.0f, 1.0f
				};

				
				uint64_t action_count = 0;
				ss.read((char*)&action_count, sizeof(uint64_t));
				for (size_t i = 0; i < action_count; ++i)
				{
					uint64_t name_length = 0;
					ss.read((char*)&name_length, sizeof(uint64_t));
					std::string name;
					name.resize(name_length);
					ss.read(name.data(), name.size());

					Action action;
					action.length = 0.0f;
					action.channels.resize(bone_count);

					for (size_t i = 0; i < bone_count; ++i)
					{
						auto&& group = action.channels[i];
						for (size_t i = 0; i < 10; ++i)
						{
							auto&& channel = group[i];
							uint64_t keyframe_count = 0;
							ss.read((char*)&keyframe_count, sizeof(uint64_t));
							for (size_t i = 0; i < keyframe_count; ++i)
							{
								float time = 0.0f;
								float value = 0.0f;
								ss.read((char*)&time, sizeof(float));
								ss.read((char*)&value, sizeof(float));
								channel.keyframes.emplace(std::make_pair(time, value));
								if (time > action.length)
									action.length = time;
							}
							if (keyframe_count == 0)
								channel.keyframes.emplace(std::make_pair(0.0f, default_channel_values[i]));
						}
					}

					animation->actions.emplace(std::make_pair(name, std::move(action)));
				}
			}

			animation->loaded = true;
		});
	t.detach();

	return animation;
}

void Animation::Pose::apply(Matrix4 * transforms) const
{
	for (size_t i = 0; i < bones.size(); ++i)
	{
		transforms[i] = bones[i].matrix * transforms[bones[i].parent];
	}
}

float Animation::Channel::operator[](float time) const
{
	auto next = keyframes.lower_bound(time);
	if (next == keyframes.end())
		--next;
	auto prev = next;
	if (prev != keyframes.begin())
		--prev;
	float diff = next->first - prev->first;
	float progress = time - prev->first;
	if (diff != 0.0f)
		progress /= diff;
	return next->second * progress + prev->second * (1.0f - progress);
}

void Animation::Action::getPose(Pose& pose, Pose& base_pose, float time) const
{
	for (size_t i = 0; i < channels.size(); ++i)
	{
		auto&& group = channels[i];
		std::array<float, 10> v;
		for (size_t i = 0; i < 10; ++i)
			v[i] = group[i][time];
		Quaternion rotation(v[3], v[4], v[5], v[6]);
		rotation.Normalize();
		pose.bones[i].matrix = Matrix4::Scale(Vec3(v[7], v[8], v[9])) * Matrix4(rotation) * Matrix4::Translation(Vec3(v[0], v[1], v[2])) * base_pose.bones[i].matrix;
	}
}
