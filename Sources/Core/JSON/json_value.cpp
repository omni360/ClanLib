/*
**  ClanLib SDK
**  Copyright (c) 1997-2015 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
*/

#include "Core/precomp.h"
#include "API/Core/JSON/json_value.h"
#include "API/Core/Text/string_help.h"

namespace clan
{

std::string JsonValue::to_json() const
{
	std::string result;
	to_json(result);
	return result;
}

void JsonValue::to_json(std::string &result) const
{
	result.clear();
	write(result);
}

void JsonValue::write(std::string &json) const
{
	switch (type)
	{
	case Type::null:
		json += "null";
		break;
	case Type::object:
		write_object(json);
		break;
	case Type::array:
		write_array(json);
		break;
	case Type::string:
		write_string(value_string, json);
		break;
	case Type::number:
		write_number(json);
		break;
	case Type::boolean:
		json += value_boolean ? "true" : "false";
		break;
	case Type::undefined:
		break;
	}
}

void JsonValue::write_array(std::string &json) const
{
	json += "[";
	for (size_t i = 0; i < items.size(); i++)
	{
		if (i > 0)
			json += ",";
		items[i].write(json);
	}
	json += "]";
}

void JsonValue::write_object(std::string &json) const
{
	json += "{";
	std::map<std::string, JsonValue>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (it != members.begin())
			json += ",";
		write_string(it->first, json);
		json += ":";
		it->second.write(json);
	}
	json += "}";
}

void JsonValue::write_string(const std::string &str, std::string &json)
{
	json.push_back('"');

	for (auto c : str)
	{
		if (c == '"' || c == '\\')
		{
			json.push_back('\\');
			json.push_back(c);
		}
		else if (c == '\b')
		{
			json.push_back('\\');
			json.push_back('b');
		}
		else if (c == '\f')
		{
			json.push_back('\\');
			json.push_back('f');
		}
		else if (c == '\n')
		{
			json.push_back('\\');
			json.push_back('n');
		}
		else if (c == '\r')
		{
			json.push_back('\\');
			json.push_back('r');
		}
		else if (c == '\t')
		{
			json.push_back('\\');
			json.push_back('t');
		}
		else if (c < 32)
		{
			json.push_back('\\');
			json.push_back('u');
			json.push_back('0');
			json.push_back('0');

			if (c >= 16)
			{
				json.push_back('1');
				c -= 16;
			}
			else
			{
				json.push_back('0');
			}

			if (c < 10)
			{
				json.push_back('0' + c);
			}
			else
			{
				json.push_back('a' + (c - 10));
			}
		}
		else
		{
			json.push_back(c);
		}
	}

	json.push_back('"');
}

void JsonValue::write_number(std::string &json) const
{
	char buf[64];
	buf[0] = 0;
	if (static_cast<double>(static_cast<int>(value_number)) == value_number)
	{
#ifdef WIN32
		_snprintf(buf, 63, "%d", (int)value_number);
#else
		snprintf(buf, 63, "%d", (int)value_number);
#endif
	}
	else
	{
#ifdef WIN32
		_snprintf(buf, 63, "%f", value_number);
#else
		snprintf(buf, 63, "%f", value_number);
#endif
	}
	buf[63] = 0;
	json += buf;
}

JsonValue JsonValue::from_json(const std::string &json)
{
	size_t pos = 0;
	return read(json, pos);
}

JsonValue JsonValue::read(const std::string &json, size_t &pos)
{
	read_whitespace(json, pos);

	if (pos == json.length())
		throw JsonException("Unexpected end of JSON data");

	switch (json[pos])
	{
	case '{':
		return read_object(json, pos);
	case '[':
		return read_array(json, pos);
	case '"':
		return read_string(json, pos);
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return read_number(json, pos);
	case 'f':
	case 't':
		return read_boolean(json, pos);
	default:
		throw JsonException("Unexpected character in JSON data");
	}
}

JsonValue JsonValue::read_object(const std::string &json, size_t &pos)
{
	JsonValue result(Type::object);

	pos++;

	read_whitespace(json, pos);

	if (pos == json.length())
		throw JsonException("Unexpected end of JSON data");

	while (pos != json.length() && json[pos] != '}')
	{
		std::string key = read_string(json, pos);

		read_whitespace(json, pos);

		if (pos == json.length())
			throw JsonException("Unexpected end of JSON data");
		else if (json[pos] != ':')
			throw JsonException("Unexpected character in JSON data");
		pos++;

		read_whitespace(json, pos);

		result.members[key] = read(json, pos);

		read_whitespace(json, pos);

		if (pos == json.length())
		{
			throw JsonException("Unexpected end of JSON data");
		}
		else if (json[pos] == '}')
		{
			pos++;
			break;
		}
		else if (json[pos] == ',')
		{
			pos++;
			read_whitespace(json, pos);
		}
		else
		{
			throw JsonException("Unexpected character in JSON data");
		}
	}

	return result;
}

JsonValue JsonValue::read_array(const std::string &json, size_t &pos)
{
	JsonValue result(Type::array);

	pos++;

	read_whitespace(json, pos);

	if (pos == json.length())
		throw JsonException("Unexpected end of JSON data");

	while (json[pos] != ']')
	{
		read_whitespace(json, pos);
		result.items.push_back(read(json, pos));
		read_whitespace(json, pos);

		if (pos == json.length())
		{
			throw JsonException("Unexpected end of JSON data");
		}
		else if (json[pos] == ']')
		{
			break;
		}
		else if (json[pos] == ',')
		{
			pos++;
			read_whitespace(json, pos);
		}
		else
		{
			throw JsonException("Unexpected character in JSON data");
		}
	}
	pos++;

	return result;
}

std::string JsonValue::read_string(const std::string &json, size_t &pos)
{
	pos++;
	if (pos == json.length())
		throw JsonException("Unexpected end of JSON data");

	std::string result;
	while (true)
	{
		if (pos == json.length())
		{
			throw JsonException("Unexpected end of JSON data");
		}
		else if (json[pos] == '"')
		{
			break;
		}
		else if (json[pos] == '\\')
		{
			pos++;
			if (pos == json.length())
				throw JsonException("Unexpected end of JSON data");

			unsigned codepoint;
			switch (json[pos])
			{
			case '"':
				result.push_back('"');
				break;
			case '\\':
				result.push_back('\\');
				break;
			case '/':
				result.push_back('/');
				break;
			case 'b':
				result.push_back('\b');
				break;
			case 'f':
				result.push_back('\f');
				break;
			case 'n':
				result.push_back('\n');
				break;
			case 'r':
				result.push_back('\r');
				break;
			case 't':
				result.push_back('\t');
				break;
			case 'u':
				if (pos + 5 > json.length())
					throw JsonException("Unexpected end of JSON data");
				
				codepoint = 0;
				for (int i = 0; i < 4; i++)
				{
					char c = json[pos + 1 + i];
					if (c >= '0' && c <= '9')
					{
						codepoint <<= 4;
						codepoint |= c - '0';
					}
					else if (c >= 'a' && c <= 'f')
					{
						codepoint <<= 4;
						codepoint |= c - 'a' + 10;
					}
					else if (c >= 'A' && c <= 'F')
					{
						codepoint <<= 4;
						codepoint |= c - 'A' + 10;
					}
					else
					{
						throw JsonException("Invalid unicode escape");
					}
				}
				result += StringHelp::unicode_to_utf8(codepoint);
				pos += 4;
				break;
			}
			pos++;
		}
		else
		{
			result.push_back(json[pos]);
			pos++;
		}
	}

	pos++;

	return result;
}

JsonValue JsonValue::read_number(const std::string &json, size_t &pos)
{
	int start_pos = pos;
	if (json[pos] == '-')
		pos++;
	while (pos < json.length() && json[pos] >= '0' && json[pos] <= '9')
		pos++;
	if (pos != json.length() && json[pos] == '.')
		pos++;
	while (pos < json.length() && json[pos] >= '0' && json[pos] <= '9')
		pos++;
	if (pos != json.length() && (json[pos] == 'e' || json[pos] == 'E'))
	{
		pos++;
		if (pos != json.length() && (json[pos] == '+' || json[pos] == '-'))
			pos++;
		while (pos < json.length() && json[pos] >= '0' && json[pos] <= '9')
			pos++;
	}
	int end_pos = pos;

	std::string number_string = json.substr(start_pos, end_pos - start_pos);
	if (number_string.empty())
		throw JsonException("Unexpected character in JSON data");

	double result = 0.0;
	sscanf(number_string.c_str(), "%lf", &result);
	return result;
}

JsonValue JsonValue::read_boolean(const std::string &json, size_t &pos)
{
	if (json[pos] == 't')
	{
		if (pos + 4 > json.length() || memcmp(&json[pos], "true", 4) != 0)
			throw JsonException("Unexpected character in JSON data");
		pos += 4;
		return JsonValue::boolean(true);
	}
	else
	{
		if (pos + 5 > json.length() || memcmp(&json[pos], "false", 5) != 0)
			throw JsonException("Unexpected character in JSON data");
		pos += 5;
		return JsonValue::boolean(false);
	}
}

void JsonValue::read_whitespace(const std::string &json, size_t &pos)
{
	while (pos != json.length() && (json[pos] == ' ' || json[pos] == '\r' ||json[pos] == '\n' || json[pos] == '\t' || json[pos] == '\f'))
		pos++;
}

}
