#pragma once

#include "Component.h"

#include "Engine.h"

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
	static Level * level;
	static Tilemap * tm;
	static Xoroshiro128Plus * rng;
	// may need to make thread_local or something if multiple engines are running off the same process
};
