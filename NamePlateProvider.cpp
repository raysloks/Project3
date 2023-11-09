#include "NamePlateProvider.h"

#include "NamePlate.h"
#include "Mob.h"
#include "MobPosHandler.h"
#include "ModelRenderSystem.h"

NamePlateProvider::~NamePlateProvider()
{
	auto namePlateLocked = namePlate.lock();
	if (namePlateLocked)
		namePlateLocked->removeSelf();
}

void NamePlateProvider::start()
{
	mob = getComponent<Mob>();
	auto namePlateLocked = std::make_shared<NamePlate>();
	mrs->ui->addChild(namePlateLocked);
	namePlate = namePlateLocked;
}

void NamePlateProvider::tick(float dt)
{
	auto namePlateLocked = namePlate.lock();
	if (namePlateLocked && mob)
	{
		namePlateLocked->setSizeAnchorOffset(Vec2(120.0f, 20.0f), mrs->worldToScreen(mob->entity->xyz + Vec3::pz * 2.0f), Vec2());
		namePlateLocked->setMaxHealth(mob->hp.cap);
		namePlateLocked->setHealth(mob->hp.evaluate(net->time));
	}
}
