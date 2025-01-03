#include "CustomBehaviour.h"

Engine * CustomBehaviour::engine;
SpriteRenderSystem * CustomBehaviour::srs;
ModelRenderSystem * CustomBehaviour::mrs;
InputSystem * CustomBehaviour::input;
CustomBehaviourSystem * CustomBehaviour::cbs;
CollisionSystem * CustomBehaviour::cs;
Level * CustomBehaviour::level;
Tilemap * CustomBehaviour::tm;
Xoroshiro128Plus * CustomBehaviour::rng;
MobPosHandler * CustomBehaviour::net;

CustomBehaviour::~CustomBehaviour()
{
}

void CustomBehaviour::start()
{
}

void CustomBehaviour::tick(float dt)
{
}
