#include "DiamondComponent.h"
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "Coal.h"
#include "CustomBehaviour.h"
#include "Collider.h"
#include "Mob.h"
#include "NetworkMob.h"
#include "CircleCollider.h"
#include "RectangleCollider.h"
#include "SpriteCollider.h"
#include "TilemapCollider.h"
#include "ModelRenderer.h"
#include "Sprite.h"
#include "BouncingBall.h"
#include "Enemy.h"
#include "Player.h"
#include "FrameRate.h"
#include "TextDisplay.h"
#include "Inspector.h"
#include "FloatingText.h"
#include "TextBox.h"
#include "HealthDisplay.h"
#include "Inventory.h"
#include "FollowCursor.h"
#include "ActionBar.h"
#include "Projectile.h"
#include "SpriteAnimator.h"
#include "Body.h"
#include "ModelAnimator.h"
const std::unordered_map<std::type_index, std::string> names = {
	{ std::type_index(typeid(CustomBehaviour)), "CustomBehaviour" },
	{ std::type_index(typeid(Collider)), "Collider" },
	{ std::type_index(typeid(Mob)), "Mob" },
	{ std::type_index(typeid(NetworkMob)), "NetworkMob" },
	{ std::type_index(typeid(CircleCollider)), "CircleCollider" },
	{ std::type_index(typeid(RectangleCollider)), "RectangleCollider" },
	{ std::type_index(typeid(SpriteCollider)), "SpriteCollider" },
	{ std::type_index(typeid(TilemapCollider)), "TilemapCollider" },
	{ std::type_index(typeid(ModelRenderer)), "ModelRenderer" },
	{ std::type_index(typeid(Sprite)), "Sprite" },
	{ std::type_index(typeid(BouncingBall)), "BouncingBall" },
	{ std::type_index(typeid(Enemy)), "Enemy" },
	{ std::type_index(typeid(Player)), "Player" },
	{ std::type_index(typeid(FrameRate)), "FrameRate" },
	{ std::type_index(typeid(TextDisplay)), "TextDisplay" },
	{ std::type_index(typeid(Inspector)), "Inspector" },
	{ std::type_index(typeid(FloatingText)), "FloatingText" },
	{ std::type_index(typeid(TextBox)), "TextBox" },
	{ std::type_index(typeid(HealthDisplay)), "HealthDisplay" },
	{ std::type_index(typeid(Inventory)), "Inventory" },
	{ std::type_index(typeid(FollowCursor)), "FollowCursor" },
	{ std::type_index(typeid(ActionBar)), "ActionBar" },
	{ std::type_index(typeid(Projectile)), "Projectile" },
	{ std::type_index(typeid(SpriteAnimator)), "SpriteAnimator" },
	{ std::type_index(typeid(Body)), "Body" },
	{ std::type_index(typeid(ModelAnimator)), "ModelAnimator" } };

const std::unordered_map<std::type_index, std::function<void(std::ostream&, Component*)>> print_funcs = {
	{ std::type_index(typeid(CustomBehaviour)), [] (std::ostream& os, Component * component) {
		auto obj = (CustomBehaviour*)component;
		os << "CustomBehaviour\n";
	} },
	{ std::type_index(typeid(Collider)), [] (std::ostream& os, Component * component) {
		auto obj = (Collider*)component;
		os << "Collider\n";
		os << " layers " << obj->layers << "\n";
	} },
	{ std::type_index(typeid(Mob)), [] (std::ostream& os, Component * component) {
		auto obj = (Mob*)component;
		os << "Mob\n";
		os << " team " << obj->team << "\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
		os << " n ";
		os << obj->n.x << " ";
		os << obj->n.y << "\n";
		os << " move ";
		os << obj->move.x << " ";
		os << obj->move.y << "\n";
		os << " anim " << obj->anim << "\n";
		os << " cooldown " << obj->cooldown << "\n";
	} },
	{ std::type_index(typeid(NetworkMob)), [] (std::ostream& os, Component * component) {
		auto obj = (NetworkMob*)component;
		os << "NetworkMob\n";
		os << " team " << obj->team << "\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
		os << " n ";
		os << obj->n.x << " ";
		os << obj->n.y << "\n";
		os << " move ";
		os << obj->move.x << " ";
		os << obj->move.y << "\n";
		os << " anim " << obj->anim << "\n";
		os << " cooldown " << obj->cooldown << "\n";
		os << " id " << obj->id << "\n";
		os << " mob_template_id " << obj->mob_template_id << "\n";
		os << " angle " << obj->angle << "\n";
		os << " animation_time_run " << obj->animation_time_run << "\n";
		os << " animation_time_idle " << obj->animation_time_idle << "\n";
		os << " animation_blend " << obj->animation_blend << "\n";
	} },
	{ std::type_index(typeid(CircleCollider)), [] (std::ostream& os, Component * component) {
		auto obj = (CircleCollider*)component;
		os << "CircleCollider\n";
		os << " layers " << obj->layers << "\n";
		os << " r " << obj->r << "\n";
	} },
	{ std::type_index(typeid(RectangleCollider)), [] (std::ostream& os, Component * component) {
		auto obj = (RectangleCollider*)component;
		os << "RectangleCollider\n";
		os << " layers " << obj->layers << "\n";
		os << " half_size ";
		os << obj->half_size.x << " ";
		os << obj->half_size.y << "\n";
	} },
	{ std::type_index(typeid(SpriteCollider)), [] (std::ostream& os, Component * component) {
		auto obj = (SpriteCollider*)component;
		os << "SpriteCollider\n";
		os << " layers " << obj->layers << "\n";
		os << " rotation " << obj->rotation << "\n";
	} },
	{ std::type_index(typeid(TilemapCollider)), [] (std::ostream& os, Component * component) {
		auto obj = (TilemapCollider*)component;
		os << "TilemapCollider\n";
		os << " layers " << obj->layers << "\n";
	} },
	{ std::type_index(typeid(ModelRenderer)), [] (std::ostream& os, Component * component) {
		auto obj = (ModelRenderer*)component;
		os << "ModelRenderer\n";
	} },
	{ std::type_index(typeid(Sprite)), [] (std::ostream& os, Component * component) {
		auto obj = (Sprite*)component;
		os << "Sprite\n";
		os << " subsprite_x " << obj->subsprite_x << "\n";
		os << " subsprite_y " << obj->subsprite_y << "\n";
		os << " sort " << obj->sort << "\n";
		os << " rotation " << obj->rotation << "\n";
		os << " scale ";
		os << obj->scale.x << " ";
		os << obj->scale.y << "\n";
		os << " flip " << obj->flip << "\n";
	} },
	{ std::type_index(typeid(BouncingBall)), [] (std::ostream& os, Component * component) {
		auto obj = (BouncingBall*)component;
		os << "BouncingBall\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << " ";
		os << obj->v.z << "\n";
		os << " elasticity " << obj->elasticity << "\n";
	} },
	{ std::type_index(typeid(Enemy)), [] (std::ostream& os, Component * component) {
		auto obj = (Enemy*)component;
		os << "Enemy\n";
		os << " team " << obj->team << "\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
		os << " n ";
		os << obj->n.x << " ";
		os << obj->n.y << "\n";
		os << " move ";
		os << obj->move.x << " ";
		os << obj->move.y << "\n";
		os << " anim " << obj->anim << "\n";
		os << " cooldown " << obj->cooldown << "\n";
	} },
	{ std::type_index(typeid(Player)), [] (std::ostream& os, Component * component) {
		auto obj = (Player*)component;
		os << "Player\n";
		os << " team " << obj->team << "\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
		os << " n ";
		os << obj->n.x << " ";
		os << obj->n.y << "\n";
		os << " move ";
		os << obj->move.x << " ";
		os << obj->move.y << "\n";
		os << " anim " << obj->anim << "\n";
		os << " cooldown " << obj->cooldown << "\n";
		os << " move_target ";
		os << obj->move_target.x << " ";
		os << obj->move_target.y << "\n";
	} },
	{ std::type_index(typeid(FrameRate)), [] (std::ostream& os, Component * component) {
		auto obj = (FrameRate*)component;
		os << "FrameRate\n";
	} },
	{ std::type_index(typeid(TextDisplay)), [] (std::ostream& os, Component * component) {
		auto obj = (TextDisplay*)component;
		os << "TextDisplay\n";
	} },
	{ std::type_index(typeid(Inspector)), [] (std::ostream& os, Component * component) {
		auto obj = (Inspector*)component;
		os << "Inspector\n";
		os << " index " << obj->index << "\n";
	} },
	{ std::type_index(typeid(FloatingText)), [] (std::ostream& os, Component * component) {
		auto obj = (FloatingText*)component;
		os << "FloatingText\n";
		os << " time " << obj->time << "\n";
		os << " duration " << obj->duration << "\n";
		os << " color ";
		os << obj->color.x << " ";
		os << obj->color.y << " ";
		os << obj->color.z << "\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << " ";
		os << obj->v.z << "\n";
		os << " a ";
		os << obj->a.x << " ";
		os << obj->a.y << " ";
		os << obj->a.z << "\n";
	} },
	{ std::type_index(typeid(TextBox)), [] (std::ostream& os, Component * component) {
		auto obj = (TextBox*)component;
		os << "TextBox\n";
		os << " active " << obj->active << "\n";
	} },
	{ std::type_index(typeid(HealthDisplay)), [] (std::ostream& os, Component * component) {
		auto obj = (HealthDisplay*)component;
		os << "HealthDisplay\n";
	} },
	{ std::type_index(typeid(Inventory)), [] (std::ostream& os, Component * component) {
		auto obj = (Inventory*)component;
		os << "Inventory\n";
		os << " visible " << obj->visible << "\n";
	} },
	{ std::type_index(typeid(FollowCursor)), [] (std::ostream& os, Component * component) {
		auto obj = (FollowCursor*)component;
		os << "FollowCursor\n";
		os << " t " << obj->t << "\n";
	} },
	{ std::type_index(typeid(ActionBar)), [] (std::ostream& os, Component * component) {
		auto obj = (ActionBar*)component;
		os << "ActionBar\n";
		os << " slot_count " << obj->slot_count << "\n";
	} },
	{ std::type_index(typeid(Projectile)), [] (std::ostream& os, Component * component) {
		auto obj = (Projectile*)component;
		os << "Projectile\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] (std::ostream& os, Component * component) {
		auto obj = (SpriteAnimator*)component;
		os << "SpriteAnimator\n";
		os << " frame_count " << obj->frame_count << "\n";
		os << " speed " << obj->speed << "\n";
		os << " time " << obj->time << "\n";
		os << " freeze " << obj->freeze << "\n";
		os << " destroy " << obj->destroy << "\n";
	} },
	{ std::type_index(typeid(Body)), [] (std::ostream& os, Component * component) {
		auto obj = (Body*)component;
		os << "Body\n";
		os << " v ";
		os << obj->v.x << " ";
		os << obj->v.y << "\n";
		os << " m " << obj->m << "\n";
	} },
	{ std::type_index(typeid(ModelAnimator)), [] (std::ostream& os, Component * component) {
		auto obj = (ModelAnimator*)component;
		os << "ModelAnimator\n";
		os << " speed " << obj->speed << "\n";
		os << " time " << obj->time << "\n";
		os << " destroy " << obj->destroy << "\n";
	} } };

const std::unordered_map<std::string, std::type_index> indices = {
	{ "CustomBehaviour", std::type_index(typeid(CustomBehaviour)) },
	{ "Collider", std::type_index(typeid(Collider)) },
	{ "Mob", std::type_index(typeid(Mob)) },
	{ "NetworkMob", std::type_index(typeid(NetworkMob)) },
	{ "CircleCollider", std::type_index(typeid(CircleCollider)) },
	{ "RectangleCollider", std::type_index(typeid(RectangleCollider)) },
	{ "SpriteCollider", std::type_index(typeid(SpriteCollider)) },
	{ "TilemapCollider", std::type_index(typeid(TilemapCollider)) },
	{ "ModelRenderer", std::type_index(typeid(ModelRenderer)) },
	{ "Sprite", std::type_index(typeid(Sprite)) },
	{ "BouncingBall", std::type_index(typeid(BouncingBall)) },
	{ "Enemy", std::type_index(typeid(Enemy)) },
	{ "Player", std::type_index(typeid(Player)) },
	{ "FrameRate", std::type_index(typeid(FrameRate)) },
	{ "TextDisplay", std::type_index(typeid(TextDisplay)) },
	{ "Inspector", std::type_index(typeid(Inspector)) },
	{ "FloatingText", std::type_index(typeid(FloatingText)) },
	{ "TextBox", std::type_index(typeid(TextBox)) },
	{ "HealthDisplay", std::type_index(typeid(HealthDisplay)) },
	{ "Inventory", std::type_index(typeid(Inventory)) },
	{ "FollowCursor", std::type_index(typeid(FollowCursor)) },
	{ "ActionBar", std::type_index(typeid(ActionBar)) },
	{ "Projectile", std::type_index(typeid(Projectile)) },
	{ "SpriteAnimator", std::type_index(typeid(SpriteAnimator)) },
	{ "Body", std::type_index(typeid(Body)) },
	{ "ModelAnimator", std::type_index(typeid(ModelAnimator)) } };

const std::unordered_map<std::type_index, std::function<Component*()>> create_funcs = {
	{ std::type_index(typeid(CustomBehaviour)), [] () {
		return new CustomBehaviour();
	} },
	{ std::type_index(typeid(Collider)), [] () {
		return new Collider();
	} },
	{ std::type_index(typeid(Mob)), [] () {
		return new Mob();
	} },
	{ std::type_index(typeid(NetworkMob)), [] () {
		return new NetworkMob();
	} },
	{ std::type_index(typeid(CircleCollider)), [] () {
		return new CircleCollider();
	} },
	{ std::type_index(typeid(RectangleCollider)), [] () {
		return new RectangleCollider();
	} },
	{ std::type_index(typeid(SpriteCollider)), [] () {
		return new SpriteCollider();
	} },
	{ std::type_index(typeid(TilemapCollider)), [] () {
		return new TilemapCollider();
	} },
	{ std::type_index(typeid(ModelRenderer)), [] () {
		return new ModelRenderer();
	} },
	{ std::type_index(typeid(Sprite)), [] () {
		return new Sprite();
	} },
	{ std::type_index(typeid(BouncingBall)), [] () {
		return new BouncingBall();
	} },
	{ std::type_index(typeid(Enemy)), [] () {
		return new Enemy();
	} },
	{ std::type_index(typeid(Player)), [] () {
		return new Player();
	} },
	{ std::type_index(typeid(FrameRate)), [] () {
		return new FrameRate();
	} },
	{ std::type_index(typeid(TextDisplay)), [] () {
		return new TextDisplay();
	} },
	{ std::type_index(typeid(Inspector)), [] () {
		return new Inspector();
	} },
	{ std::type_index(typeid(FloatingText)), [] () {
		return new FloatingText();
	} },
	{ std::type_index(typeid(TextBox)), [] () {
		return new TextBox();
	} },
	{ std::type_index(typeid(HealthDisplay)), [] () {
		return new HealthDisplay();
	} },
	{ std::type_index(typeid(Inventory)), [] () {
		return new Inventory();
	} },
	{ std::type_index(typeid(FollowCursor)), [] () {
		return new FollowCursor();
	} },
	{ std::type_index(typeid(ActionBar)), [] () {
		return new ActionBar();
	} },
	{ std::type_index(typeid(Projectile)), [] () {
		return new Projectile();
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] () {
		return new SpriteAnimator();
	} },
	{ std::type_index(typeid(Body)), [] () {
		return new Body();
	} },
	{ std::type_index(typeid(ModelAnimator)), [] () {
		return new ModelAnimator();
	} } };

const std::unordered_map<std::type_index, std::function<void(Component*,const Coal&)>> init_funcs = {
	{ std::type_index(typeid(CustomBehaviour)), [] (Component * component, const Coal& coal) {
		auto obj = (CustomBehaviour*)component;
	} },
	{ std::type_index(typeid(Collider)), [] (Component * component, const Coal& coal) {
		auto obj = (Collider*)component;
	} },
	{ std::type_index(typeid(Mob)), [] (Component * component, const Coal& coal) {
		auto obj = (Mob*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->n.x = coal.members.at("n").members.at("x").real;
		obj->n.y = coal.members.at("n").members.at("y").real;
		obj->move.x = coal.members.at("move").members.at("x").real;
		obj->move.y = coal.members.at("move").members.at("y").real;
		obj->anim = coal.members.at("anim").real;
		obj->cooldown = coal.members.at("cooldown").real;
	} },
	{ std::type_index(typeid(NetworkMob)), [] (Component * component, const Coal& coal) {
		auto obj = (NetworkMob*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->n.x = coal.members.at("n").members.at("x").real;
		obj->n.y = coal.members.at("n").members.at("y").real;
		obj->move.x = coal.members.at("move").members.at("x").real;
		obj->move.y = coal.members.at("move").members.at("y").real;
		obj->anim = coal.members.at("anim").real;
		obj->cooldown = coal.members.at("cooldown").real;
		obj->angle = coal.members.at("angle").real;
		obj->animation_time_run = coal.members.at("animation_time_run").real;
		obj->animation_time_idle = coal.members.at("animation_time_idle").real;
		obj->animation_blend = coal.members.at("animation_blend").real;
	} },
	{ std::type_index(typeid(CircleCollider)), [] (Component * component, const Coal& coal) {
		auto obj = (CircleCollider*)component;
		obj->r = coal.members.at("r").real;
	} },
	{ std::type_index(typeid(RectangleCollider)), [] (Component * component, const Coal& coal) {
		auto obj = (RectangleCollider*)component;
		obj->half_size.x = coal.members.at("half_size").members.at("x").real;
		obj->half_size.y = coal.members.at("half_size").members.at("y").real;
	} },
	{ std::type_index(typeid(SpriteCollider)), [] (Component * component, const Coal& coal) {
		auto obj = (SpriteCollider*)component;
		obj->rotation = coal.members.at("rotation").real;
	} },
	{ std::type_index(typeid(TilemapCollider)), [] (Component * component, const Coal& coal) {
		auto obj = (TilemapCollider*)component;
	} },
	{ std::type_index(typeid(ModelRenderer)), [] (Component * component, const Coal& coal) {
		auto obj = (ModelRenderer*)component;
	} },
	{ std::type_index(typeid(Sprite)), [] (Component * component, const Coal& coal) {
		auto obj = (Sprite*)component;
		obj->sort = coal.members.at("sort").real;
		obj->rotation = coal.members.at("rotation").real;
		obj->scale.x = coal.members.at("scale").members.at("x").real;
		obj->scale.y = coal.members.at("scale").members.at("y").real;
	} },
	{ std::type_index(typeid(BouncingBall)), [] (Component * component, const Coal& coal) {
		auto obj = (BouncingBall*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->v.z = coal.members.at("v").members.at("z").real;
		obj->elasticity = coal.members.at("elasticity").real;
	} },
	{ std::type_index(typeid(Enemy)), [] (Component * component, const Coal& coal) {
		auto obj = (Enemy*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->n.x = coal.members.at("n").members.at("x").real;
		obj->n.y = coal.members.at("n").members.at("y").real;
		obj->move.x = coal.members.at("move").members.at("x").real;
		obj->move.y = coal.members.at("move").members.at("y").real;
		obj->anim = coal.members.at("anim").real;
		obj->cooldown = coal.members.at("cooldown").real;
	} },
	{ std::type_index(typeid(Player)), [] (Component * component, const Coal& coal) {
		auto obj = (Player*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->n.x = coal.members.at("n").members.at("x").real;
		obj->n.y = coal.members.at("n").members.at("y").real;
		obj->move.x = coal.members.at("move").members.at("x").real;
		obj->move.y = coal.members.at("move").members.at("y").real;
		obj->anim = coal.members.at("anim").real;
		obj->cooldown = coal.members.at("cooldown").real;
		obj->move_target.x = coal.members.at("move_target").members.at("x").real;
		obj->move_target.y = coal.members.at("move_target").members.at("y").real;
	} },
	{ std::type_index(typeid(FrameRate)), [] (Component * component, const Coal& coal) {
		auto obj = (FrameRate*)component;
	} },
	{ std::type_index(typeid(TextDisplay)), [] (Component * component, const Coal& coal) {
		auto obj = (TextDisplay*)component;
	} },
	{ std::type_index(typeid(Inspector)), [] (Component * component, const Coal& coal) {
		auto obj = (Inspector*)component;
	} },
	{ std::type_index(typeid(FloatingText)), [] (Component * component, const Coal& coal) {
		auto obj = (FloatingText*)component;
		obj->time = coal.members.at("time").real;
		obj->duration = coal.members.at("duration").real;
		obj->color.x = coal.members.at("color").members.at("x").real;
		obj->color.y = coal.members.at("color").members.at("y").real;
		obj->color.z = coal.members.at("color").members.at("z").real;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->v.z = coal.members.at("v").members.at("z").real;
		obj->a.x = coal.members.at("a").members.at("x").real;
		obj->a.y = coal.members.at("a").members.at("y").real;
		obj->a.z = coal.members.at("a").members.at("z").real;
	} },
	{ std::type_index(typeid(TextBox)), [] (Component * component, const Coal& coal) {
		auto obj = (TextBox*)component;
		obj->active = coal.members.at("active").boolean;
	} },
	{ std::type_index(typeid(HealthDisplay)), [] (Component * component, const Coal& coal) {
		auto obj = (HealthDisplay*)component;
	} },
	{ std::type_index(typeid(Inventory)), [] (Component * component, const Coal& coal) {
		auto obj = (Inventory*)component;
		obj->visible = coal.members.at("visible").boolean;
	} },
	{ std::type_index(typeid(FollowCursor)), [] (Component * component, const Coal& coal) {
		auto obj = (FollowCursor*)component;
		obj->t = coal.members.at("t").real;
	} },
	{ std::type_index(typeid(ActionBar)), [] (Component * component, const Coal& coal) {
		auto obj = (ActionBar*)component;
	} },
	{ std::type_index(typeid(Projectile)), [] (Component * component, const Coal& coal) {
		auto obj = (Projectile*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] (Component * component, const Coal& coal) {
		auto obj = (SpriteAnimator*)component;
		obj->speed = coal.members.at("speed").real;
		obj->time = coal.members.at("time").real;
		obj->freeze = coal.members.at("freeze").real;
		obj->destroy = coal.members.at("destroy").boolean;
	} },
	{ std::type_index(typeid(Body)), [] (Component * component, const Coal& coal) {
		auto obj = (Body*)component;
		obj->v.x = coal.members.at("v").members.at("x").real;
		obj->v.y = coal.members.at("v").members.at("y").real;
		obj->m = coal.members.at("m").real;
	} },
	{ std::type_index(typeid(ModelAnimator)), [] (Component * component, const Coal& coal) {
		auto obj = (ModelAnimator*)component;
		obj->speed = coal.members.at("speed").real;
		obj->time = coal.members.at("time").real;
		obj->destroy = coal.members.at("destroy").boolean;
	} } };

const std::unordered_map<std::type_index, std::function<Coal(Component*)>> data_funcs = {
	{ std::type_index(typeid(CustomBehaviour)), [] (Component * component) {
		auto obj = (CustomBehaviour*)component;
		Coal coal;
		return coal;
	} },
	{ std::type_index(typeid(Collider)), [] (Component * component) {
		auto obj = (Collider*)component;
		Coal coal;
		coal.members["layers"] = (uint64_t)obj->layers;
		return coal;
	} },
	{ std::type_index(typeid(Mob)), [] (Component * component) {
		auto obj = (Mob*)component;
		Coal coal;
		coal.members["team"] = (uint64_t)obj->team;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		coal.members["n"] = Coal({
			{"x", obj->n.x},
			{"y", obj->n.y}});
		coal.members["move"] = Coal({
			{"x", obj->move.x},
			{"y", obj->move.y}});
		coal.members["anim"] = obj->anim;
		coal.members["cooldown"] = obj->cooldown;
		return coal;
	} },
	{ std::type_index(typeid(NetworkMob)), [] (Component * component) {
		auto obj = (NetworkMob*)component;
		Coal coal;
		coal.members["team"] = (uint64_t)obj->team;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		coal.members["n"] = Coal({
			{"x", obj->n.x},
			{"y", obj->n.y}});
		coal.members["move"] = Coal({
			{"x", obj->move.x},
			{"y", obj->move.y}});
		coal.members["anim"] = obj->anim;
		coal.members["cooldown"] = obj->cooldown;
		coal.members["id"] = (uint64_t)obj->id;
		coal.members["mob_template_id"] = (uint64_t)obj->mob_template_id;
		coal.members["angle"] = obj->angle;
		coal.members["animation_time_run"] = obj->animation_time_run;
		coal.members["animation_time_idle"] = obj->animation_time_idle;
		coal.members["animation_blend"] = obj->animation_blend;
		return coal;
	} },
	{ std::type_index(typeid(CircleCollider)), [] (Component * component) {
		auto obj = (CircleCollider*)component;
		Coal coal;
		coal.members["layers"] = (uint64_t)obj->layers;
		coal.members["r"] = obj->r;
		return coal;
	} },
	{ std::type_index(typeid(RectangleCollider)), [] (Component * component) {
		auto obj = (RectangleCollider*)component;
		Coal coal;
		coal.members["layers"] = (uint64_t)obj->layers;
		coal.members["half_size"] = Coal({
			{"x", obj->half_size.x},
			{"y", obj->half_size.y}});
		return coal;
	} },
	{ std::type_index(typeid(SpriteCollider)), [] (Component * component) {
		auto obj = (SpriteCollider*)component;
		Coal coal;
		coal.members["layers"] = (uint64_t)obj->layers;
		coal.members["rotation"] = obj->rotation;
		return coal;
	} },
	{ std::type_index(typeid(TilemapCollider)), [] (Component * component) {
		auto obj = (TilemapCollider*)component;
		Coal coal;
		coal.members["layers"] = (uint64_t)obj->layers;
		return coal;
	} },
	{ std::type_index(typeid(ModelRenderer)), [] (Component * component) {
		auto obj = (ModelRenderer*)component;
		Coal coal;
		return coal;
	} },
	{ std::type_index(typeid(Sprite)), [] (Component * component) {
		auto obj = (Sprite*)component;
		Coal coal;
		coal.members["subsprite_x"] = (uint64_t)obj->subsprite_x;
		coal.members["subsprite_y"] = (uint64_t)obj->subsprite_y;
		coal.members["sort"] = obj->sort;
		coal.members["rotation"] = obj->rotation;
		coal.members["scale"] = Coal({
			{"x", obj->scale.x},
			{"y", obj->scale.y}});
		coal.members["flip"] = (uint64_t)obj->flip;
		return coal;
	} },
	{ std::type_index(typeid(BouncingBall)), [] (Component * component) {
		auto obj = (BouncingBall*)component;
		Coal coal;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y},
			{"z", obj->v.z}});
		coal.members["elasticity"] = obj->elasticity;
		return coal;
	} },
	{ std::type_index(typeid(Enemy)), [] (Component * component) {
		auto obj = (Enemy*)component;
		Coal coal;
		coal.members["team"] = (uint64_t)obj->team;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		coal.members["n"] = Coal({
			{"x", obj->n.x},
			{"y", obj->n.y}});
		coal.members["move"] = Coal({
			{"x", obj->move.x},
			{"y", obj->move.y}});
		coal.members["anim"] = obj->anim;
		coal.members["cooldown"] = obj->cooldown;
		return coal;
	} },
	{ std::type_index(typeid(Player)), [] (Component * component) {
		auto obj = (Player*)component;
		Coal coal;
		coal.members["team"] = (uint64_t)obj->team;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		coal.members["n"] = Coal({
			{"x", obj->n.x},
			{"y", obj->n.y}});
		coal.members["move"] = Coal({
			{"x", obj->move.x},
			{"y", obj->move.y}});
		coal.members["anim"] = obj->anim;
		coal.members["cooldown"] = obj->cooldown;
		coal.members["move_target"] = Coal({
			{"x", obj->move_target.x},
			{"y", obj->move_target.y}});
		return coal;
	} },
	{ std::type_index(typeid(FrameRate)), [] (Component * component) {
		auto obj = (FrameRate*)component;
		Coal coal;
		return coal;
	} },
	{ std::type_index(typeid(TextDisplay)), [] (Component * component) {
		auto obj = (TextDisplay*)component;
		Coal coal;
		return coal;
	} },
	{ std::type_index(typeid(Inspector)), [] (Component * component) {
		auto obj = (Inspector*)component;
		Coal coal;
		coal.members["index"] = (uint64_t)obj->index;
		return coal;
	} },
	{ std::type_index(typeid(FloatingText)), [] (Component * component) {
		auto obj = (FloatingText*)component;
		Coal coal;
		coal.members["time"] = obj->time;
		coal.members["duration"] = obj->duration;
		coal.members["color"] = Coal({
			{"x", obj->color.x},
			{"y", obj->color.y},
			{"z", obj->color.z}});
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y},
			{"z", obj->v.z}});
		coal.members["a"] = Coal({
			{"x", obj->a.x},
			{"y", obj->a.y},
			{"z", obj->a.z}});
		return coal;
	} },
	{ std::type_index(typeid(TextBox)), [] (Component * component) {
		auto obj = (TextBox*)component;
		Coal coal;
		coal.members["active"] = obj->active;
		return coal;
	} },
	{ std::type_index(typeid(HealthDisplay)), [] (Component * component) {
		auto obj = (HealthDisplay*)component;
		Coal coal;
		return coal;
	} },
	{ std::type_index(typeid(Inventory)), [] (Component * component) {
		auto obj = (Inventory*)component;
		Coal coal;
		coal.members["visible"] = obj->visible;
		return coal;
	} },
	{ std::type_index(typeid(FollowCursor)), [] (Component * component) {
		auto obj = (FollowCursor*)component;
		Coal coal;
		coal.members["t"] = obj->t;
		return coal;
	} },
	{ std::type_index(typeid(ActionBar)), [] (Component * component) {
		auto obj = (ActionBar*)component;
		Coal coal;
		coal.members["slot_count"] = (uint64_t)obj->slot_count;
		return coal;
	} },
	{ std::type_index(typeid(Projectile)), [] (Component * component) {
		auto obj = (Projectile*)component;
		Coal coal;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		return coal;
	} },
	{ std::type_index(typeid(SpriteAnimator)), [] (Component * component) {
		auto obj = (SpriteAnimator*)component;
		Coal coal;
		coal.members["frame_count"] = (uint64_t)obj->frame_count;
		coal.members["speed"] = obj->speed;
		coal.members["time"] = obj->time;
		coal.members["freeze"] = obj->freeze;
		coal.members["destroy"] = obj->destroy;
		return coal;
	} },
	{ std::type_index(typeid(Body)), [] (Component * component) {
		auto obj = (Body*)component;
		Coal coal;
		coal.members["v"] = Coal({
			{"x", obj->v.x},
			{"y", obj->v.y}});
		coal.members["m"] = obj->m;
		return coal;
	} },
	{ std::type_index(typeid(ModelAnimator)), [] (Component * component) {
		auto obj = (ModelAnimator*)component;
		Coal coal;
		coal.members["speed"] = obj->speed;
		coal.members["time"] = obj->time;
		coal.members["destroy"] = obj->destroy;
		return coal;
	} } };

void Diamond::print(std::ostream& os, Component * component)
{
	print_funcs.at(std::type_index(typeid(*component)))(os, component);
}

std::type_index Diamond::index(const std::string& type)
{
	return indices.at(type);
}

std::string Diamond::name(Component * component)
{
	return names.at(std::type_index(typeid(*component)));
}

Component * Diamond::create(const std::type_index& type)
{
	return create_funcs.at(type)();
}

void Diamond::init(Component * component, const Coal& coal)
{
	init_funcs.at(std::type_index(typeid(*component)))(component, coal);
}

Coal Diamond::data(Component * component)
{
	return data_funcs.at(std::type_index(typeid(*component)))(component);
}

