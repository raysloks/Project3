#include "Diamond.h"
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "Component.h"
#include "CustomBehaviour.h"
#include "Player.h"
#include "Mob.h"
#include "ActionBar.h"
#include "TilemapCollider.h"
#include "Collider.h"
#include "SpriteCollider.h"
#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "Sprite.h"
#include "Body.h"
#include "BouncingBall.h"
#include "Enemy.h"
#include "FloatingText.h"
#include "FollowCursor.h"
#include "FrameRate.h"
#include "HealthDisplay.h"
#include "Inventory.h"
#include "SpriteAnimator.h"
#include "Projectile.h"
#include "NetworkMob.h"
#include "Inspector.h"
#include "TextDisplay.h"
#include "TextBox.h"
const std::unordered_map<std::type_index, std::string> names = {
{ std::type_index(typeid(Component)), "Component" },
{ std::type_index(typeid(CustomBehaviour)), "CustomBehaviour" },
{ std::type_index(typeid(Player)), "Player" },
{ std::type_index(typeid(Mob)), "Mob" },
{ std::type_index(typeid(ActionBar)), "ActionBar" },
{ std::type_index(typeid(TilemapCollider)), "TilemapCollider" },
{ std::type_index(typeid(Collider)), "Collider" },
{ std::type_index(typeid(SpriteCollider)), "SpriteCollider" },
{ std::type_index(typeid(CircleCollider)), "CircleCollider" },
{ std::type_index(typeid(RectangleCollider)), "RectangleCollider" },
{ std::type_index(typeid(Sprite)), "Sprite" },
{ std::type_index(typeid(Body)), "Body" },
{ std::type_index(typeid(BouncingBall)), "BouncingBall" },
{ std::type_index(typeid(Enemy)), "Enemy" },
{ std::type_index(typeid(FloatingText)), "FloatingText" },
{ std::type_index(typeid(FollowCursor)), "FollowCursor" },
{ std::type_index(typeid(FrameRate)), "FrameRate" },
{ std::type_index(typeid(HealthDisplay)), "HealthDisplay" },
{ std::type_index(typeid(Inventory)), "Inventory" },
{ std::type_index(typeid(SpriteAnimator)), "SpriteAnimator" },
{ std::type_index(typeid(Projectile)), "Projectile" },
{ std::type_index(typeid(NetworkMob)), "NetworkMob" },
{ std::type_index(typeid(Inspector)), "Inspector" },
{ std::type_index(typeid(TextDisplay)), "TextDisplay" },
{ std::type_index(typeid(TextBox)), "TextBox" } };
const std::unordered_map<std::type_index, std::function<void(Component*)>> funcs = {
	{ std::type_index(typeid(Component)), [] (Component * component) {
		auto obj = (Component*)component;
	} },
	{ std::type_index(typeid(CustomBehaviour)), [] (Component * component) {
		auto obj = (CustomBehaviour*)component;
	} },
	{ std::type_index(typeid(Player)), [] (Component * component) {
		auto obj = (Player*)component;
		std::cout << "hp " << obj->hp << std::endl;
		std::cout << "hp_max " << obj->hp_max << std::endl;
		std::cout << "anim " << obj->anim << std::endl;
		std::cout << "cooldown " << obj->cooldown << std::endl;
	} },
	{ std::type_index(typeid(Mob)), [] (Component * component) {
		auto obj = (Mob*)component;
		std::cout << "hp " << obj->hp << std::endl;
		std::cout << "hp_max " << obj->hp_max << std::endl;
		std::cout << "anim " << obj->anim << std::endl;
		std::cout << "cooldown " << obj->cooldown << std::endl;
	} },
	{ std::type_index(typeid(ActionBar)), [] (Component * component) {
		auto obj = (ActionBar*)component;
		std::cout << "slot_count " << obj->slot_count << std::endl;
	} },
	{ std::type_index(typeid(TilemapCollider)), [] (Component * component) {
		auto obj = (TilemapCollider*)component;
		std::cout << "layers " << obj->layers << std::endl;
		std::cout << "tilemap " << obj->tilemap << std::endl;
	} },
	{ std::type_index(typeid(Collider)), [] (Component * component) {
		auto obj = (Collider*)component;
		std::cout << "layers " << obj->layers << std::endl;
	} },
	{ std::type_index(typeid(SpriteCollider)), [] (Component * component) {
		auto obj = (SpriteCollider*)component;
		std::cout << "layers " << obj->layers << std::endl;
		std::cout << "rotation " << obj->rotation << std::endl;
	} },
	{ std::type_index(typeid(CircleCollider)), [] (Component * component) {
		auto obj = (CircleCollider*)component;
		std::cout << "layers " << obj->layers << std::endl;
		std::cout << "r " << obj->r << std::endl;
	} },
	{ std::type_index(typeid(RectangleCollider)), [] (Component * component) {
		auto obj = (RectangleCollider*)component;
		std::cout << "layers " << obj->layers << std::endl;
	} },
	{ std::type_index(typeid(Sprite)), [] (Component * component) {
		auto obj = (Sprite*)component;
		std::cout << "subsprite_x " << obj->subsprite_x << std::endl;
		std::cout << "subsprite_y " << obj->subsprite_y << std::endl;
		std::cout << "sort " << obj->sort << std::endl;
		std::cout << "rotation " << obj->rotation << std::endl;
		std::cout << "flip " << obj->flip << std::endl;
	} },
	{ std::type_index(typeid(Body)), [] (Component * component) {
		auto obj = (Body*)component;
		std::cout << "m " << obj->m << std::endl;
	} },
	{ std::type_index(typeid(BouncingBall)), [] (Component * component) {
		auto obj = (BouncingBall*)component;
		std::cout << "elasticity " << obj->elasticity << std::endl;
	} },
	{ std::type_index(typeid(Enemy)), [] (Component * component) {
		auto obj = (Enemy*)component;
		std::cout << "hp " << obj->hp << std::endl;
		std::cout << "hp_max " << obj->hp_max << std::endl;
		std::cout << "anim " << obj->anim << std::endl;
		std::cout << "cooldown " << obj->cooldown << std::endl;
	} },
	{ std::type_index(typeid(FloatingText)), [] (Component * component) {
		auto obj = (FloatingText*)component;
		std::cout << "time " << obj->time << std::endl;
		std::cout << "duration " << obj->duration << std::endl;
	} },
	{ std::type_index(typeid(FollowCursor)), [] (Component * component) {
		auto obj = (FollowCursor*)component;
		std::cout << "t " << obj->t << std::endl;
	} },
	{ std::type_index(typeid(FrameRate)), [] (Component * component) {
		auto obj = (FrameRate*)component;
		std::cout << "max_frame_count " << obj->max_frame_count << std::endl;
	} },
	{ std::type_index(typeid(HealthDisplay)), [] (Component * component) {
		auto obj = (HealthDisplay*)component;
	} },
	{ std::type_index(typeid(Inventory)), [] (Component * component) {
		auto obj = (Inventory*)component;
		std::cout << "visible " << obj->visible << std::endl;
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] (Component * component) {
		auto obj = (SpriteAnimator*)component;
		std::cout << "frame_count " << obj->frame_count << std::endl;
		std::cout << "speed " << obj->speed << std::endl;
		std::cout << "time " << obj->time << std::endl;
		std::cout << "freeze " << obj->freeze << std::endl;
		std::cout << "destroy " << obj->destroy << std::endl;
	} },
	{ std::type_index(typeid(Projectile)), [] (Component * component) {
		auto obj = (Projectile*)component;
	} },
	{ std::type_index(typeid(NetworkMob)), [] (Component * component) {
		auto obj = (NetworkMob*)component;
		std::cout << "hp " << obj->hp << std::endl;
		std::cout << "hp_max " << obj->hp_max << std::endl;
		std::cout << "anim " << obj->anim << std::endl;
		std::cout << "cooldown " << obj->cooldown << std::endl;
	} },
	{ std::type_index(typeid(Inspector)), [] (Component * component) {
		auto obj = (Inspector*)component;
		std::cout << "index " << obj->index << std::endl;
	} },
	{ std::type_index(typeid(TextDisplay)), [] (Component * component) {
		auto obj = (TextDisplay*)component;
	} },
	{ std::type_index(typeid(TextBox)), [] (Component * component) {
		auto obj = (TextBox*)component;
	} } };
const std::unordered_map<std::type_index, std::function<std::string(Component*)>> data_funcs = {
	{ std::type_index(typeid(Component)), [] (Component * component) {
		auto obj = (Component*)component;
		std::stringstream ss;
		ss << "Component\n";
		return ss.str();
	} },
	{ std::type_index(typeid(CustomBehaviour)), [] (Component * component) {
		auto obj = (CustomBehaviour*)component;
		std::stringstream ss;
		ss << "CustomBehaviour\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Player)), [] (Component * component) {
		auto obj = (Player*)component;
		std::stringstream ss;
		ss << "Player\n";
		ss << " hp " << obj->hp << "\n";
		ss << " hp_max " << obj->hp_max << "\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		ss << " n ";
		ss << obj->n.x << " ";
		ss << obj->n.y << "\n";
		ss << " move ";
		ss << obj->move.x << " ";
		ss << obj->move.y << "\n";
		ss << " anim " << obj->anim << "\n";
		ss << " cooldown " << obj->cooldown << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Mob)), [] (Component * component) {
		auto obj = (Mob*)component;
		std::stringstream ss;
		ss << "Mob\n";
		ss << " hp " << obj->hp << "\n";
		ss << " hp_max " << obj->hp_max << "\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		ss << " n ";
		ss << obj->n.x << " ";
		ss << obj->n.y << "\n";
		ss << " move ";
		ss << obj->move.x << " ";
		ss << obj->move.y << "\n";
		ss << " anim " << obj->anim << "\n";
		ss << " cooldown " << obj->cooldown << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(ActionBar)), [] (Component * component) {
		auto obj = (ActionBar*)component;
		std::stringstream ss;
		ss << "ActionBar\n";
		ss << " slot_count " << obj->slot_count << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(TilemapCollider)), [] (Component * component) {
		auto obj = (TilemapCollider*)component;
		std::stringstream ss;
		ss << "TilemapCollider\n";
		ss << " layers " << obj->layers << "\n";
		ss << " tilemap " << obj->tilemap << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Collider)), [] (Component * component) {
		auto obj = (Collider*)component;
		std::stringstream ss;
		ss << "Collider\n";
		ss << " layers " << obj->layers << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(SpriteCollider)), [] (Component * component) {
		auto obj = (SpriteCollider*)component;
		std::stringstream ss;
		ss << "SpriteCollider\n";
		ss << " layers " << obj->layers << "\n";
		ss << " rotation " << obj->rotation << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(CircleCollider)), [] (Component * component) {
		auto obj = (CircleCollider*)component;
		std::stringstream ss;
		ss << "CircleCollider\n";
		ss << " layers " << obj->layers << "\n";
		ss << " r " << obj->r << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(RectangleCollider)), [] (Component * component) {
		auto obj = (RectangleCollider*)component;
		std::stringstream ss;
		ss << "RectangleCollider\n";
		ss << " layers " << obj->layers << "\n";
		ss << " half_size ";
		ss << obj->half_size.x << " ";
		ss << obj->half_size.y << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Sprite)), [] (Component * component) {
		auto obj = (Sprite*)component;
		std::stringstream ss;
		ss << "Sprite\n";
		ss << " subsprite_x " << obj->subsprite_x << "\n";
		ss << " subsprite_y " << obj->subsprite_y << "\n";
		ss << " sort " << obj->sort << "\n";
		ss << " rotation " << obj->rotation << "\n";
		ss << " scale ";
		ss << obj->scale.x << " ";
		ss << obj->scale.y << "\n";
		ss << " flip " << obj->flip << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Body)), [] (Component * component) {
		auto obj = (Body*)component;
		std::stringstream ss;
		ss << "Body\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		ss << " m " << obj->m << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(BouncingBall)), [] (Component * component) {
		auto obj = (BouncingBall*)component;
		std::stringstream ss;
		ss << "BouncingBall\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << " ";
		ss << obj->v.z << "\n";
		ss << " elasticity " << obj->elasticity << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Enemy)), [] (Component * component) {
		auto obj = (Enemy*)component;
		std::stringstream ss;
		ss << "Enemy\n";
		ss << " hp " << obj->hp << "\n";
		ss << " hp_max " << obj->hp_max << "\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		ss << " n ";
		ss << obj->n.x << " ";
		ss << obj->n.y << "\n";
		ss << " move ";
		ss << obj->move.x << " ";
		ss << obj->move.y << "\n";
		ss << " anim " << obj->anim << "\n";
		ss << " cooldown " << obj->cooldown << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(FloatingText)), [] (Component * component) {
		auto obj = (FloatingText*)component;
		std::stringstream ss;
		ss << "FloatingText\n";
		ss << " time " << obj->time << "\n";
		ss << " duration " << obj->duration << "\n";
		ss << " color ";
		ss << obj->color.x << " ";
		ss << obj->color.y << " ";
		ss << obj->color.z << "\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << " ";
		ss << obj->v.z << "\n";
		ss << " a ";
		ss << obj->a.x << " ";
		ss << obj->a.y << " ";
		ss << obj->a.z << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(FollowCursor)), [] (Component * component) {
		auto obj = (FollowCursor*)component;
		std::stringstream ss;
		ss << "FollowCursor\n";
		ss << " t " << obj->t << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(FrameRate)), [] (Component * component) {
		auto obj = (FrameRate*)component;
		std::stringstream ss;
		ss << "FrameRate\n";
		ss << " max_frame_count " << obj->max_frame_count << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(HealthDisplay)), [] (Component * component) {
		auto obj = (HealthDisplay*)component;
		std::stringstream ss;
		ss << "HealthDisplay\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Inventory)), [] (Component * component) {
		auto obj = (Inventory*)component;
		std::stringstream ss;
		ss << "Inventory\n";
		ss << " visible " << obj->visible << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] (Component * component) {
		auto obj = (SpriteAnimator*)component;
		std::stringstream ss;
		ss << "SpriteAnimator\n";
		ss << " frame_count " << obj->frame_count << "\n";
		ss << " speed " << obj->speed << "\n";
		ss << " time " << obj->time << "\n";
		ss << " freeze " << obj->freeze << "\n";
		ss << " destroy " << obj->destroy << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Projectile)), [] (Component * component) {
		auto obj = (Projectile*)component;
		std::stringstream ss;
		ss << "Projectile\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(NetworkMob)), [] (Component * component) {
		auto obj = (NetworkMob*)component;
		std::stringstream ss;
		ss << "NetworkMob\n";
		ss << " hp " << obj->hp << "\n";
		ss << " hp_max " << obj->hp_max << "\n";
		ss << " v ";
		ss << obj->v.x << " ";
		ss << obj->v.y << "\n";
		ss << " n ";
		ss << obj->n.x << " ";
		ss << obj->n.y << "\n";
		ss << " move ";
		ss << obj->move.x << " ";
		ss << obj->move.y << "\n";
		ss << " anim " << obj->anim << "\n";
		ss << " cooldown " << obj->cooldown << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(Inspector)), [] (Component * component) {
		auto obj = (Inspector*)component;
		std::stringstream ss;
		ss << "Inspector\n";
		ss << " index " << obj->index << "\n";
		return ss.str();
	} },
	{ std::type_index(typeid(TextDisplay)), [] (Component * component) {
		auto obj = (TextDisplay*)component;
		std::stringstream ss;
		ss << "TextDisplay\n";
		return ss.str();
	} },
	{ std::type_index(typeid(TextBox)), [] (Component * component) {
		auto obj = (TextBox*)component;
		std::stringstream ss;
		ss << "TextBox\n";
		return ss.str();
	} } };
void print(Component * component)
{
	std::cout << names.at(std::type_index(typeid(*component))) << std::endl;
	funcs.at(std::type_index(typeid(*component)))(component);
}
std::string getDataOf(Component * component)
{
	return data_funcs.at(std::type_index(typeid(*component)))(component);
}
