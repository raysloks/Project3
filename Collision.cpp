#include "Collision.h"

void Collision::flip()
{
	n = -n;
	auto temp = collider;
	collider = other;
	other = temp;
}
