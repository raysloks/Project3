#pragma once

#include "Component.h"

#include "Engine.h"
#include "SpriteRenderSystem.h"
#include "InputSystem.h"
#include "CustomBehaviourSystem.h"
#include "CollisionSystem.h"

class CustomBehaviour :
	public Component
{
public:
	virtual ~CustomBehaviour();

	virtual void tick(float dt);
	
	// static storage of system pointers to allow more convenient access for custom behaviours
	static thread_local Engine * engine;
	static thread_local SpriteRenderSystem * srs;
	static thread_local InputSystem * input;
	static thread_local CustomBehaviourSystem * cbs;
	static thread_local CollisionSystem * cs;
};
