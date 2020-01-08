#include "Projectile.h"

void Projectile::tick(float dt)
{
	entity->p += v * dt;
}
