#include "KeyBindings.h"

void KeyBindings::set(uint64_t action, uint64_t key)
{
	actionToKey[action] = key;
	keyToAction[key] = action;
}

uint64_t KeyBindings::getAction(uint64_t key) const
{
	auto i = keyToAction.find(key);
	if (i != keyToAction.end())
		return i->second;
	return std::numeric_limits<uint32_t>::max();
}

uint64_t KeyBindings::getKey(uint64_t action) const
{
	auto i = actionToKey.find(action);
	if (i != actionToKey.end())
		return i->second;
	return std::numeric_limits<uint32_t>::max();
}
