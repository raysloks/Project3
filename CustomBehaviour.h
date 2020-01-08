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

	virtual void start();

	virtual void tick(float dt);
	
	// static storage of system pointers to allow more convenient access for custom behaviours
	static Engine * engine;
	static SpriteRenderSystem * srs;
	static InputSystem * input;
	static CustomBehaviourSystem * cbs;
	static CollisionSystem * cs;
	// may need to make thread_local or something if multiple engines are running off the same process
};
