#include "Coal.h"

#include <iostream>
#include <sstream>

Coal parseString(std::istream& is)
{
	std::string string;
	int c;
	do
	{
		c = is.get();
		if (c == '\\')
		{
			c = is.get();
			string += c;
			continue;
		}
		if (c == '"')
		{
			/*do
			{
				c = is.get();
				if (isspace(c))
					continue;
				if (c == '"')
					break;
				is.unget();
				return string;
			} while (!is.eof());
			continue;*/
			break;
		}
		string += c;
	} while (!is.eof());
	return string;
}

Coal parseNumber(std::istream& is)
{
	std::string string;
	bool radix = false;
	int c;
	do
	{
		c = is.get();
		if (isdigit(c))
		{
			string += c;
			continue;
		}
		if (radix == false)
		{
			if (c == '.')
			{
				string += c;
				radix = true;
				continue;
			}
		}
		if (c == 'f')
		{
			radix = true;
			break;
		}
		is.unget();
		break;
	} while (!is.eof());

	std::stringstream ss(string);
	if (radix)
	{
		float real;
		ss >> real;
		return real;
	}
	else
	{
		int64_t integer;
		ss >> integer;
		return integer;
	}
}

std::string parseName(std::istream& is)
{
	int c;
	do
	{
		c = is.get();
		if (isspace(c))
			continue;
		if (isalpha(c) || c == '_')
		{
			std::string name;
			name += c;
			do
			{
				c = is.get();
				if (isalnum(c) || c == '_')
					name += c;
				else
					break;
			} while (!is.eof());
			return name;
		}
		break;
	} while (!is.eof());
	return "<error>";
}

Coal parseMembers(std::istream& is)
{
	Coal coal;
	coal.type = Coal::Type::Object;
	do
	{
		auto name = parseName(is);
		if (name != "<error>")
		{
			auto member = Coal::parse(is);
			if (member)
			{
				coal.members.emplace(std::make_pair(name, *member));
			}
			else
			{
				throw std::runtime_error("Unable to parse member '" + name + "'.");
			}
		}
	} while (!is.eof());
	return coal;
}

Coal parseElements(std::istream& is)
{
	Coal coal;
	coal.type = Coal::Type::Array;
	do
	{
		auto element = Coal::parse(is);
		if (element)
		{
			coal.elements.push_back(*element);
		}
		else
		{
			break;
		}
	} while (!is.eof());
	return coal;
}

Coal::Coal(std::nullptr_t)
{
	integer = 0;
	real = 0.0f;
	boolean = false;
	type = Type::Null;
}

Coal::Coal(const char * string) : string(string)
{
	integer = 0;
	real = 0.0f;
	boolean = false;
	type = Type::String;
}

Coal::Coal(const std::string& string) : string(string)
{
	integer = 0;
	real = 0.0f;
	boolean = false;
	type = Type::String;
}

Coal::Coal(int64_t integer) : integer(integer)
{
	real = integer;
	boolean = integer;
	type = Type::Integer;
}

Coal::Coal(uint64_t integer) : integer(integer)
{
	real = integer;
	boolean = integer;
	type = Type::Integer;
}

Coal::Coal(float real) : real(real)
{
	integer = real;
	boolean = real;
	type = Type::Real;
}

Coal::Coal(bool boolean) : boolean(boolean)
{
	integer = boolean;
	real = boolean;
	type = Type::Boolean;
}

Coal::Coal(const std::map<std::string, Coal>& members) : members(members)
{
	integer = 0;
	real = 0.0f;
	boolean = false;
	type = Type::Object;
}

Coal::Coal(const std::vector<Coal>& elements) : elements(elements)
{
	integer = 0;
	real = 0.0f;
	boolean = false;
	type = Type::Array;
}

std::optional<Coal> Coal::parse(std::istream& is)
{
	int c;
	do
	{
		c = is.get();
		if (isspace(c))
		{
			continue;
		}
		if (c == '"')
		{
			return parseString(is);
		}
		if (isalpha(c))
		{
			std::string string;
			string += c;
			do
			{
				c = is.get();
				if (isalpha(c))
					string += c;
				else
					break;
			} while (!is.eof());
			if (string == "true")
			{
				return true;
			}
			if (string == "false")
			{
				return false;
			}
			if (string == "null")
			{
				return nullptr;
			}
			throw std::runtime_error("Unrecognized symbol '" + string + "'.");
		}
		if (isdigit(c) || c == '-')
		{
			is.unget();
			return parseNumber(is);
		}
		if (c == '{')
		{
			return parseMembers(is);
		}
		if (c == '[')
		{
			return parseElements(is);
		}
		break;
	} while (!is.eof());
	return std::optional<Coal>();
}

std::string escape(const std::string& string)
{
	std::string escaped;
	for (auto c : string)
	{
		switch (c)
		{
		case '"':
			escaped += '\\';
			break;
		case '\\':
			escaped += '\\';
			break;
		default:
			break;
		}
		escaped += c;
	}
	return escaped;
}

void printCoal(std::ostream& os, const Coal& coal, const std::string& indent)
{
	switch (coal.type)
	{
	case Coal::Type::Null:
		os << "null";
		break;
	case Coal::Type::String:
		os << "\"" << escape(coal.string) << "\"";
		break;
	case Coal::Type::Integer:
		os << coal.integer;
		break;
	case Coal::Type::Real:
		os << coal.real << "f";
		break;
	case Coal::Type::Boolean:
		os << (coal.boolean ? "true" : "false");
		break;
	case Coal::Type::Object:
		os << "{\n";
		for (auto&& member : coal.members)
		{
			os << indent + " " << member.first << " ";
			printCoal(os, member.second, indent + " ");
		}
		os << indent << "}";
		break;
	case Coal::Type::Array:
		os << "[\n";
		for (auto&& element : coal.elements)
		{
			os << indent + " ";
			printCoal(os, element, indent + " ");
			/*if (&element != &coal.elements.back())
				os << indent << " ,\n";*/
		}
		os << indent << "]";
		break;
	default:
		break;
	}
	os << '\n';
}

void Coal::print(std::ostream& os) const
{
	printCoal(os, *this, "");
}

bool Coal::operator==(const Coal& rhs) const
{
	if (type != rhs.type)
		return false;
	switch (type)
	{
	case Coal::Type::Null:
		return true;
	case Coal::Type::String:
		return string == rhs.string;
	case Coal::Type::Integer:
		return integer == rhs.integer;
	case Coal::Type::Real:
		return real == rhs.real;
	case Coal::Type::Boolean:
		return boolean == rhs.boolean;
	case Coal::Type::Object:
		return members == rhs.members;
	case Coal::Type::Array:
		return elements == rhs.elements;
	default:
		return false;
	}
}

bool Coal::operator!=(const Coal& rhs) const
{
	if (type != rhs.type)
		return true;
	switch (type)
	{
	case Coal::Type::Null:
		return false;
	case Coal::Type::String:
		return string != rhs.string;
	case Coal::Type::Integer:
		return integer != rhs.integer;
	case Coal::Type::Real:
		return real != rhs.real;
	case Coal::Type::Boolean:
		return boolean != rhs.boolean;
	case Coal::Type::Object:
		return members != rhs.members;
	case Coal::Type::Array:
		return elements != rhs.elements;
	default:
		return false;
	}
}

Coal& Coal::operator[](const std::string& key)
{
	return members[key];
}

Coal& Coal::operator[](std::string&& key)
{
	return members[key];
}

Coal& Coal::operator[](size_t index)
{
	return elements[index];
}

const Coal & Coal::operator[](size_t index) const
{
	return elements[index];
}
