#pragma once

#include <string>
#include <map>
#include <vector>
#include <optional>

class Coal
{
public:
	Coal() = default;
	Coal(std::nullptr_t);
	Coal(const char * string);
	Coal(const std::string& string);
	Coal(int64_t integer);
	Coal(uint64_t integer);
	Coal(int32_t integer);
	Coal(uint32_t integer);
	Coal(float real);
	Coal(bool boolean);
	Coal(const std::map<std::string, Coal>& members);
	Coal(const std::vector<Coal>& elements);

	static std::optional<Coal> parse(std::istream& is);

	void print(std::ostream& os) const;

	enum class Type
	{
		Null,
		String,
		Integer,
		Real,
		Boolean,
		Object,
		Array
	};

	Type type;
	std::string string;
	int64_t integer;
	float real;
	bool boolean;
	std::map<std::string, Coal> members;
	std::vector<Coal> elements;

	bool operator==(const Coal& rhs) const;
	bool operator!=(const Coal& rhs) const;

	Coal& operator[](const std::string& key);
	Coal& operator[](std::string&& key);
	Coal& operator[](size_t index);
	const Coal& operator[](size_t index) const;
};

