#pragma once

#include <map>

class KeyBindings
{
public:

	void set(uint64_t action, uint64_t key);

	uint64_t getAction(uint64_t key) const;
	uint64_t getKey(uint64_t action) const;

	std::map<uint64_t, uint64_t> keyToAction, actionToKey;
};

