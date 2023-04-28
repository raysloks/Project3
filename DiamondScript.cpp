#include "DiamondScript.h"
#include <iostream>
#include <sstream>
#include "ScriptCode.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "MobStatBlock.h"
#include "ModelRenderer.h"
#include "Quaternion.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Window.h"
#include "WindowEvents.h"
DestructorThunk(Quaternion)
DestructorThunk(Matrix3)
DestructorThunk(Matrix4)
DestructorThunk(Vec4)
DestructorThunk(ModelRenderer)
DestructorThunk(TextInputEvent)
DestructorThunk(Window)
ThunkResolver(Window, onEvent)
void Diamond::populate(ScriptCompile& comp)
{
	comp.classes["MobStatBlock"] = std::make_shared<ScriptClassData>("MobStatBlock", sizeof(MobStatBlock));
	comp.classes["Vec2"] = std::make_shared<ScriptClassData>("Vec2", sizeof(Vec2));
	comp.classes["Quaternion"] = std::make_shared<ScriptClassData>("Quaternion", sizeof(Quaternion));
	comp.classes["Matrix3"] = std::make_shared<ScriptClassData>("Matrix3", sizeof(Matrix3));
	comp.classes["Matrix4"] = std::make_shared<ScriptClassData>("Matrix4", sizeof(Matrix4));
	comp.classes["Vec3"] = std::make_shared<ScriptClassData>("Vec3", sizeof(Vec3));
	comp.classes["Vec4"] = std::make_shared<ScriptClassData>("Vec4", sizeof(Vec4));
	comp.classes["ModelRenderer"] = std::make_shared<ScriptClassData>("ModelRenderer", sizeof(ModelRenderer), true);
	comp.classes["WindowEvent"] = std::make_shared<ScriptClassData>("WindowEvent", sizeof(WindowEvent));
	comp.classes["TextInputEvent"] = std::make_shared<ScriptClassData>("TextInputEvent", sizeof(TextInputEvent));
	comp.classes["KeyUpEvent"] = std::make_shared<ScriptClassData>("KeyUpEvent", sizeof(KeyUpEvent));
	comp.classes["KeyDownEvent"] = std::make_shared<ScriptClassData>("KeyDownEvent", sizeof(KeyDownEvent));
	comp.classes["Window"] = std::make_shared<ScriptClassData>("Window", sizeof(Window), true);

	{
		auto& class_data = comp.classes["MobStatBlock"];
		class_data->AddMember("hp", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, hp));
		class_data->AddMember("armor", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, armor));
		class_data->AddMember("strength", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, strength));
		class_data->AddMember("agility", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, agility));
		class_data->AddMember("intelligence", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, intelligence));
		class_data->AddMember("attack_power", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, attack_power));
		class_data->AddMember("spell_power", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, spell_power));
		class_data->AddMember("movement_speed", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, movement_speed));
		class_data->AddMember("attack_speed", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, attack_speed));
		class_data->AddMember("cast_speed", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, cast_speed));
		class_data->AddMember("cooldown_rate", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, cooldown_rate));
		class_data->AddMember("critical_strike_chance", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, critical_strike_chance));
		class_data->AddMember("critical_strike_multiplier", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, critical_strike_multiplier));
		class_data->AddMember("tenacity", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, tenacity));
		class_data->AddMember("stability", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, stability));
		class_data->AddMember("duration", NewScriptTypeData<int64_t>(), offsetof(MobStatBlock, duration));
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::array<__int64,32>"];
			class_data->AddMember("stat", script_type_data, offsetof(MobStatBlock, stat));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("add", function_prototype, (MobStatBlock&(MobStatBlock::*)(const MobStatBlock&))&MobStatBlock::add);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("increase", function_prototype, (MobStatBlock&(MobStatBlock::*)(const MobStatBlock&))&MobStatBlock::increase);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["MobStatBlock"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("more", function_prototype, (MobStatBlock&(MobStatBlock::*)(const MobStatBlock&))&MobStatBlock::more);
		}
	}

	{
		auto& class_data = comp.classes["Vec2"];
		class_data->AddMember("x", NewScriptTypeData<float>(), offsetof(Vec2, x));
		class_data->AddMember("y", NewScriptTypeData<float>(), offsetof(Vec2, y));
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("Len", function_prototype, (float(Vec2::*)()const)&Vec2::Len);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("LenPwr", function_prototype, (float(Vec2::*)()const)&Vec2::LenPwr);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Normalize", function_prototype, (Vec2&(Vec2::*)())&Vec2::Normalize);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Normalized", function_prototype, (Vec2(Vec2::*)()const)&Vec2::Normalized);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Dot", function_prototype, (float(Vec2::*)(const Vec2&)const)&Vec2::Dot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("Truncate", function_prototype, (Vec2&(Vec2::*)(float))&Vec2::Truncate);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("Truncated", function_prototype, (Vec2(Vec2::*)(float)const)&Vec2::Truncated);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("Rotate", function_prototype, (Vec2&(Vec2::*)(float))&Vec2::Rotate);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+=", function_prototype, (const Vec2&(Vec2::*)(const Vec2&))&Vec2::operator+=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-=", function_prototype, (const Vec2&(Vec2::*)(const Vec2&))&Vec2::operator-=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*=", function_prototype, (const Vec2&(Vec2::*)(float))&Vec2::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec2&(Vec2::*)(const Vec2&))&Vec2::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/=", function_prototype, (const Vec2&(Vec2::*)(float))&Vec2::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator/=", function_prototype, (const Vec2&(Vec2::*)(const Vec2&))&Vec2::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Vec2(Vec2::*)(const Vec2&)const)&Vec2::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-", function_prototype, (Vec2(Vec2::*)(const Vec2&)const)&Vec2::operator-);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Vec2(Vec2::*)(float)const)&Vec2::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec2(Vec2::*)(const Vec2&)const)&Vec2::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/", function_prototype, (Vec2(Vec2::*)(float)const)&Vec2::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator/", function_prototype, (Vec2(Vec2::*)(const Vec2&)const)&Vec2::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator!=", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator!=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator==", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator==);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator<);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<=", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator<=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator>);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>=", function_prototype, (bool(Vec2::*)(const Vec2&)const)&Vec2::operator>=);
		}
	}

	{
		auto& class_data = comp.classes["Quaternion"];
		class_data->AddMember("w", NewScriptTypeData<float>(), offsetof(Quaternion, w));
		class_data->AddMember("x", NewScriptTypeData<float>(), offsetof(Quaternion, x));
		class_data->AddMember("y", NewScriptTypeData<float>(), offsetof(Quaternion, y));
		class_data->AddMember("z", NewScriptTypeData<float>(), offsetof(Quaternion, z));
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("~Quaternion", function_prototype, GetDestructorThunk(Quaternion));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Quaternion(Quaternion::*)(float)const)&Quaternion::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Quaternion&(Quaternion::*)(const Quaternion&))&Quaternion::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-", function_prototype, (Quaternion(Quaternion::*)(const Quaternion&)const)&Quaternion::operator-);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Dot", function_prototype, (float(Quaternion::*)(const Quaternion&)const)&Quaternion::Dot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("Normalize", function_prototype, (void(Quaternion::*)())&Quaternion::Normalize);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Quaternion"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getConj", function_prototype, (Quaternion(Quaternion::*)()const)&Quaternion::getConj);
		}
	}

	{
		auto& class_data = comp.classes["Matrix3"];
		class_data->AddMember("data", NewScriptTypeData<float>(), offsetof(Matrix3, data));
		class_data->AddMember("mtrx", NewScriptTypeData<float>(), offsetof(Matrix3, mtrx));
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("~Matrix3", function_prototype, GetDestructorThunk(Matrix3));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("Determinant", function_prototype, (float(Matrix3::*)()const)&Matrix3::Determinant);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Inverse", function_prototype, (Matrix3(Matrix3::*)()const)&Matrix3::Inverse);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Transpose", function_prototype, (Matrix3(Matrix3::*)()const)&Matrix3::Transpose);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*=", function_prototype, (const Matrix3&(Matrix3::*)(float))&Matrix3::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/=", function_prototype, (const Matrix3&(Matrix3::*)(float))&Matrix3::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Matrix3(Matrix3::*)(const Matrix3&)const)&Matrix3::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec3(Matrix3::*)(const Vec3&)const)&Matrix3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Matrix3(Matrix3::*)(const Matrix3&)const)&Matrix3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Matrix3(Matrix3::*)(float)const)&Matrix3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator==", function_prototype, (bool(Matrix3::*)(const Matrix3&)const)&Matrix3::operator==);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator!=", function_prototype, (bool(Matrix3::*)(const Matrix3&)const)&Matrix3::operator!=);
		}
	}

	{
		auto& class_data = comp.classes["Matrix4"];
		class_data->AddMember("data", NewScriptTypeData<float>(), offsetof(Matrix4, data));
		class_data->AddMember("mtrx", NewScriptTypeData<float>(), offsetof(Matrix4, mtrx));
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("~Matrix4", function_prototype, GetDestructorThunk(Matrix4));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("Determinant", function_prototype, (float(Matrix4::*)()const)&Matrix4::Determinant);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Inverse", function_prototype, (Matrix4(Matrix4::*)()const)&Matrix4::Inverse);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Transpose", function_prototype, (Matrix4(Matrix4::*)()const)&Matrix4::Transpose);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Matrix4(Matrix4::*)(const Matrix4&)const)&Matrix4::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Matrix4(Matrix4::*)(float)const)&Matrix4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Matrix4(Matrix4::*)(const Matrix4&)const)&Matrix4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+=", function_prototype, (const Matrix4&(Matrix4::*)(const Matrix4&))&Matrix4::operator+=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*=", function_prototype, (const Matrix4&(Matrix4::*)(float))&Matrix4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Matrix4&(Matrix4::*)(const Matrix4&))&Matrix4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/", function_prototype, (Matrix4(Matrix4::*)(float)const)&Matrix4::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/=", function_prototype, (const Matrix4&(Matrix4::*)(float))&Matrix4::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator!=", function_prototype, (bool(Matrix4::*)(const Matrix4&)const)&Matrix4::operator!=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator==", function_prototype, (bool(Matrix4::*)(const Matrix4&)const)&Matrix4::operator==);
		}
	}

	{
		auto& class_data = comp.classes["Vec3"];
		class_data->AddMember("x", NewScriptTypeData<float>(), offsetof(Vec3, x));
		class_data->AddMember("y", NewScriptTypeData<float>(), offsetof(Vec3, y));
		class_data->AddMember("z", NewScriptTypeData<float>(), offsetof(Vec3, z));
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("xy", script_type_data, offsetof(Vec3, xy));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("Len", function_prototype, (float(Vec3::*)()const)&Vec3::Len);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("LenPwr", function_prototype, (float(Vec3::*)()const)&Vec3::LenPwr);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Normalize", function_prototype, (Vec3&(Vec3::*)())&Vec3::Normalize);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("Normalized", function_prototype, (Vec3(Vec3::*)()const)&Vec3::Normalized);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Dot", function_prototype, (float(Vec3::*)(const Vec3&)const)&Vec3::Dot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Cross", function_prototype, (Vec3(Vec3::*)(const Vec3&)const)&Vec3::Cross);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+=", function_prototype, (const Vec3&(Vec3::*)(const Vec3&))&Vec3::operator+=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-=", function_prototype, (const Vec3&(Vec3::*)(const Vec3&))&Vec3::operator-=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec3&(Vec3::*)(const Matrix4&))&Vec3::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec3&(Vec3::*)(const Matrix3&))&Vec3::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*=", function_prototype, (const Vec3&(Vec3::*)(float))&Vec3::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec3&(Vec3::*)(const Vec3&))&Vec3::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/=", function_prototype, (const Vec3&(Vec3::*)(float))&Vec3::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator/=", function_prototype, (const Vec3&(Vec3::*)(const Vec3&))&Vec3::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Vec3(Vec3::*)(const Vec3&)const)&Vec3::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-", function_prototype, (Vec3(Vec3::*)(const Vec3&)const)&Vec3::operator-);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec3(Vec3::*)(const Matrix4&)const)&Vec3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec3(Vec3::*)(const Matrix3&)const)&Vec3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Vec3(Vec3::*)(float)const)&Vec3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec3(Vec3::*)(const Vec3&)const)&Vec3::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/", function_prototype, (Vec3(Vec3::*)(float)const)&Vec3::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator/", function_prototype, (Vec3(Vec3::*)(const Vec3&)const)&Vec3::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator!=", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator!=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator==", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator==);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator<);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<=", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator<=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator>);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>=", function_prototype, (bool(Vec3::*)(const Vec3&)const)&Vec3::operator>=);
		}
	}

	{
		auto& class_data = comp.classes["Vec4"];
		class_data->AddMember("x", NewScriptTypeData<float>(), offsetof(Vec4, x));
		class_data->AddMember("y", NewScriptTypeData<float>(), offsetof(Vec4, y));
		class_data->AddMember("z", NewScriptTypeData<float>(), offsetof(Vec4, z));
		class_data->AddMember("w", NewScriptTypeData<float>(), offsetof(Vec4, w));
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec3"];
			class_data->AddMember("xyz", script_type_data, offsetof(Vec4, xyz));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("xy", script_type_data, offsetof(Vec4, xy));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("~Vec4", function_prototype, GetDestructorThunk(Vec4));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("operator class Vec2", function_prototype, (Vec2(Vec4::*)())&Vec4::operator class Vec2);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec3"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("operator class Vec3", function_prototype, (Vec3(Vec4::*)())&Vec4::operator class Vec3);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("Len", function_prototype, (float(Vec4::*)()const)&Vec4::Len);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			class_data->AddFunction("LenPwr", function_prototype, (float(Vec4::*)()const)&Vec4::LenPwr);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("Normalize", function_prototype, (void(Vec4::*)())&Vec4::Normalize);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<float>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Dot", function_prototype, (float(Vec4::*)(const Vec4&)const)&Vec4::Dot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("Cross", function_prototype, (Vec4(Vec4::*)(const Vec4&)const)&Vec4::Cross);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+=", function_prototype, (const Vec4&(Vec4::*)(const Vec4&))&Vec4::operator+=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-=", function_prototype, (const Vec4&(Vec4::*)(const Vec4&))&Vec4::operator-=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec4&(Vec4::*)(const Matrix4&))&Vec4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec4&(Vec4::*)(const Matrix3&))&Vec4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*=", function_prototype, (const Vec4&(Vec4::*)(float))&Vec4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*=", function_prototype, (const Vec4&(Vec4::*)(const Vec4&))&Vec4::operator*=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator/=", function_prototype, (const Vec4&(Vec4::*)(const Matrix4&))&Vec4::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/=", function_prototype, (const Vec4&(Vec4::*)(float))&Vec4::operator/=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator+", function_prototype, (Vec4(Vec4::*)(const Vec4&)const)&Vec4::operator+);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator-", function_prototype, (Vec4(Vec4::*)(const Vec4&)const)&Vec4::operator-);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec4(Vec4::*)(const Matrix4&)const)&Vec4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Matrix3"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec4(Vec4::*)(const Matrix3&)const)&Vec4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator*", function_prototype, (Vec4(Vec4::*)(float)const)&Vec4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator*", function_prototype, (Vec4(Vec4::*)(const Vec4&)const)&Vec4::operator*);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.ret = script_type_data;
			}
			function_prototype.params.push_back(NewScriptTypeData<float>());
			class_data->AddFunction("operator/", function_prototype, (Vec4(Vec4::*)(float)const)&Vec4::operator/);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator!=", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator!=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator==", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator==);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator<);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator<=", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator<=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator>);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec4"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator>=", function_prototype, (bool(Vec4::*)(const Vec4&)const)&Vec4::operator>=);
		}
	}

	{
		auto& class_data = comp.classes["ModelRenderer"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Reference<Entity>"];
			class_data->AddMember("entity", script_type_data, offsetof(ModelRenderer, entity));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["ModelRenderer::UniformBufferObject"];
			class_data->AddMember("uniform_buffer_object", script_type_data, offsetof(ModelRenderer, uniform_buffer_object));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Animation::Pose"];
			class_data->AddMember("pose", script_type_data, offsetof(ModelRenderer, pose));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::shared_ptr<Model>"];
			class_data->AddMember("model", script_type_data, offsetof(ModelRenderer, model));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::shared_ptr<SpriteSheet>"];
			class_data->AddMember("texture", script_type_data, offsetof(ModelRenderer, texture));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::shared_ptr<Animation>"];
			class_data->AddMember("animation", script_type_data, offsetof(ModelRenderer, animation));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Matrix4"];
			class_data->AddMember("transform", script_type_data, offsetof(ModelRenderer, transform));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddVirtualFunction("~ModelRenderer", function_prototype, GetDestructorThunk(ModelRenderer));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["std::shared_ptr<RenderingModel>"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RenderContext"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("getRenderingModel", function_prototype, (std::shared_ptr<RenderingModel>(ModelRenderer::*)(const RenderContext&))&ModelRenderer::getRenderingModel);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RenderContext"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("updateUniformBuffer", function_prototype, (void(ModelRenderer::*)(const RenderContext&))&ModelRenderer::updateUniformBuffer);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("setDirty", function_prototype, (void(ModelRenderer::*)())&ModelRenderer::setDirty);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<uint64_t>();
			class_data->AddFunction("getUniformBufferObjectSize", function_prototype, (uint64_t(ModelRenderer::*)()const)&ModelRenderer::getUniformBufferObjectSize);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["ModelRenderer"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["ModelRenderer"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator=", function_prototype, (ModelRenderer&(ModelRenderer::*)(const ModelRenderer&))&ModelRenderer::operator=);
		}
	}

	{
		auto& class_data = comp.classes["WindowEvent"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("cursor_position", script_type_data, offsetof(WindowEvent, cursor_position));
		}
	}

	{
		auto& class_data = comp.classes["TextInputEvent"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("cursor_position", script_type_data, offsetof(TextInputEvent, cursor_position));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::string"];
			class_data->AddMember("text", script_type_data, offsetof(TextInputEvent, text));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddFunction("~TextInputEvent", function_prototype, GetDestructorThunk(TextInputEvent));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["TextInputEvent"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["TextInputEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator=", function_prototype, (TextInputEvent&(TextInputEvent::*)(TextInputEvent&&))&TextInputEvent::operator=);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["TextInputEvent"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["TextInputEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator=", function_prototype, (TextInputEvent&(TextInputEvent::*)(const TextInputEvent&))&TextInputEvent::operator=);
		}
	}

	{
		auto& class_data = comp.classes["KeyUpEvent"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("cursor_position", script_type_data, offsetof(KeyUpEvent, cursor_position));
		}
		class_data->AddMember("key", NewScriptTypeData<uint64_t>(), offsetof(KeyUpEvent, key));
		class_data->AddMember("mod", NewScriptTypeData<uint64_t>(), offsetof(KeyUpEvent, mod));
	}

	{
		auto& class_data = comp.classes["KeyDownEvent"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("cursor_position", script_type_data, offsetof(KeyDownEvent, cursor_position));
		}
		class_data->AddMember("key", NewScriptTypeData<uint64_t>(), offsetof(KeyDownEvent, key));
		class_data->AddMember("mod", NewScriptTypeData<uint64_t>(), offsetof(KeyDownEvent, mod));
		class_data->AddMember("repeat", NewScriptTypeData<bool>(), offsetof(KeyDownEvent, repeat));
	}

	{
		auto& class_data = comp.classes["Window"];
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("minAnchor", script_type_data, offsetof(Window, minAnchor));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("maxAnchor", script_type_data, offsetof(Window, maxAnchor));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("minOffset", script_type_data, offsetof(Window, minOffset));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["Vec2"];
			class_data->AddMember("maxOffset", script_type_data, offsetof(Window, maxOffset));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::shared_ptr<ModelRenderer>"];
			class_data->AddMember("model", script_type_data, offsetof(Window, model));
		}
		class_data->AddMember("hidden", NewScriptTypeData<bool>(), offsetof(Window, hidden));
		class_data->AddMember("clickable", NewScriptTypeData<bool>(), offsetof(Window, clickable));
		class_data->AddMember("clipping", NewScriptTypeData<bool>(), offsetof(Window, clipping));
		class_data->AddMember("cursor", NewScriptTypeData<uint64_t>(), offsetof(Window, cursor));
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 0;
			script_type_data.class_data = comp.classes["std::vector<std::shared_ptr<Window>,std::allocator<std::shared_ptr<Window>>>"];
			class_data->AddMember("children", script_type_data, offsetof(Window, children));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 1;
			script_type_data.class_data = comp.classes["Window"];
			class_data->AddMember("parent", script_type_data, offsetof(Window, parent));
		}
		{
			ScriptTypeData script_type_data;
			script_type_data.type = ST_CLASS;
			script_type_data.indirection = 1;
			script_type_data.class_data = comp.classes["RootWindow"];
			class_data->AddMember("root", script_type_data, offsetof(Window, root));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			class_data->AddVirtualFunction("~Window", function_prototype, GetDestructorThunk(Window));
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["std::shared_ptr<Window>"];
				function_prototype.params.push_back(script_type_data);
			}
			function_prototype.params.push_back(NewScriptTypeData<int64_t>());
			class_data->AddFunction("addChild", function_prototype, (void(Window::*)(const std::shared_ptr<Window>&, int64_t))&Window::addChild);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["std::shared_ptr<Window>"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("removeChild", function_prototype, (void(Window::*)(const std::shared_ptr<Window>&))&Window::removeChild);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Window"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getParent", function_prototype, (Window*(Window::*)()const)&Window::getParent);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RootWindow"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getRoot", function_prototype, (RootWindow*(Window::*)()const)&Window::getRoot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getMin", function_prototype, (Vec2(Window::*)()const)&Window::getMin);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getMax", function_prototype, (Vec2(Window::*)()const)&Window::getMax);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.ret = script_type_data;
			}
			class_data->AddFunction("getCenter", function_prototype, (Vec2(Window::*)()const)&Window::getCenter);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("setAnchorOffset", function_prototype, (void(Window::*)(const Vec2&, const Vec2&, const Vec2&, const Vec2&))&Window::setAnchorOffset);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("setSizeAnchorOffset", function_prototype, (void(Window::*)(const Vec2&, const Vec2&, const Vec2&))&Window::setSizeAnchorOffset);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RenderContext"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("getRenderingModels", function_prototype, (void(Window::*)(RenderContext&))&Window::getRenderingModels);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RenderContext"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("updateUniformBuffers", function_prototype, (void(Window::*)(const RenderContext&))&Window::updateUniformBuffers);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("containsPosition", function_prototype, (bool(Window::*)(const Vec2&)const)&Window::containsPosition);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 0;
				script_type_data.class_data = comp.classes["std::shared_ptr<Window>"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Vec2"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("getAtPosition", function_prototype, (std::shared_ptr<Window>(Window::*)(const Vec2&))&Window::getAtPosition);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["FocusLostEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const FocusLostEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["FocusGainedEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const FocusGainedEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["CursorLeaveEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const CursorLeaveEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["CursorEnterEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const CursorEnterEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["CursorMoveEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const CursorMoveEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["TextInputEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const TextInputEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["KeyUpEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const KeyUpEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<bool>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["KeyDownEvent"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddVirtualFunction("onEvent", function_prototype, ResolveThunk(Window, onEvent, bool, const KeyDownEvent&)());
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			function_prototype.ret = NewScriptTypeData<void>();
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["RootWindow"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("setRoot", function_prototype, (void(Window::*)(RootWindow*))&Window::setRoot);
		}
		{
			ScriptFunctionPrototype function_prototype;
			function_prototype.cc = CC_MICROSOFT_X64;
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Window"];
				function_prototype.ret = script_type_data;
			}
			{
				ScriptTypeData script_type_data;
				script_type_data.type = ST_CLASS;
				script_type_data.indirection = 1;
				script_type_data.class_data = comp.classes["Window"];
				function_prototype.params.push_back(script_type_data);
			}
			class_data->AddFunction("operator=", function_prototype, (Window&(Window::*)(const Window&))&Window::operator=);
		}
	}

}
