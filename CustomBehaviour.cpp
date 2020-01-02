#include "CustomBehaviour.h"

Engine * CustomBehaviour::engine;
SpriteRenderSystem * CustomBehaviour::srs;
InputSystem * CustomBehaviour::input;
CustomBehaviourSystem * CustomBehaviour::cbs;
CollisionSystem * CustomBehaviour::cs;

CustomBehaviour::~CustomBehaviour()
{
}

void CustomBehaviour::tick(float dt)
{
}
