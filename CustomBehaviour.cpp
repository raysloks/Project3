#include "CustomBehaviour.h"

thread_local Engine * CustomBehaviour::engine;
thread_local SpriteRenderSystem * CustomBehaviour::srs;
thread_local InputSystem * CustomBehaviour::input;
thread_local CustomBehaviourSystem * CustomBehaviour::cbs;
thread_local CollisionSystem * CustomBehaviour::cs;

CustomBehaviour::~CustomBehaviour()
{
}

void CustomBehaviour::tick(float dt)
{
}
