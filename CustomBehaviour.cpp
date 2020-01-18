#include "CustomBehaviour.h"

Engine * CustomBehaviour::engine;
SpriteRenderSystem * CustomBehaviour::srs;
InputSystem * CustomBehaviour::input;
CustomBehaviourSystem * CustomBehaviour::cbs;
CollisionSystem * CustomBehaviour::cs;
Level * CustomBehaviour::level;
Tilemap * CustomBehaviour::tm;
Xoroshiro128Plus * CustomBehaviour::rng;

CustomBehaviour::~CustomBehaviour()
{
}

void CustomBehaviour::start()
{
}

void CustomBehaviour::tick(float dt)
{
}
