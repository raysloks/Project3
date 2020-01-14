#include "Projectile.h"

void Projectile::tick(float dt)
{
	entity->xy += v * dt;
}
