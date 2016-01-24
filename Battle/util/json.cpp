#include "json.h"
#include <assert.h>
#include <cstring>

#ifndef WIN32
#define _stricmp strcasecmp
#endif

using namespace json;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Value::Value(const Value& v) : mValueType(v.mValueType)
{
	switch (mValueType)
	{
		case StringVal		: mStringVal = v.mStringVal; break;
		case IntVal			: mIntVal = v.mIntVal; break;
		case FloatVal		: mFloatVal = v.mFloatVal; break;
		case BoolVal		: mBoolVal = v.mBoolVal; break;
		case ObjectVal		: mObjectVal = v.mObjectVal; break;
		case ArrayVal		: mArrayVal = v.mArrayVal; break;
		default				: break;
	}
}

Value& Value::operator =(const Value& v)
{
	if (&v == this)
		return *this;

	mValueType = v.mValueType;

	switch (mValueType)
	{
		case StringVal		: mStringVal = v.mStringVal; break;
		case IntVal			: mIntVal = v.mIntVal; break;
		case FloatVal		: mFloatVal = v.mFloatVal; break;
		case BoolVal		: mBoolVal = v.mBoolVal; break;
		case ObjectVal		: mObjectVal = v.mObjectVal; break;
		case ArrayVal		: mArrayVal = v.mArrayVal; break;
		default				: break;
	}

	return *this;
}

Value& Value::operator [](size_t idx)
{
	assert(mValueType == ArrayVal);
	return mArrayVal[idx];
}

Value& Value::operator [](const std::string& key)
{
	assert(mValueType == ObjectVal);
	return mObjectVal[key];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Array::Array()
{
}

Array::Array(const Array& a) : mValues(a.mValues)
{
}

Array& Array::operator =(const Array& a)
{
	if (&a == this)
		return *this;

	mValues = a.mValues;

	return *this;
}

Value& Array::operator [](size_t i)
{
	return mValues[i];
}

Array::ValueVector::const_iterator Array::begin() const
{
	return mValues.begin();
}

Array::ValueVector::const_iterator Array::end() const
{
	return mValues.end();
}

Array::ValueVector::iterator Array::begin()
{
	return mValues.begin();
}

Array::ValueVector::iterator Array::end()
{
	return mValues.end();
}

void Array::push_back(const Value& v)
{
	mValues.push_back(v);
}

void Array::insert(size_t index, const Value& v)
{
	mValues.insert(mValues.begin() + index, v);
}

size_t Array::size() const
{
	return mValues.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Object::Object()
{
}

Object::Object(const Object& obj)
{
	mValues = obj.mValues;
}

Object& Object::operator =(const Object& obj)
{
	if (&obj == this)
		return *this;

	mValues = obj.mValues;

	return *this;
}

Value& Object::operator [](const std::string& key)
{
	return mValues[key];
}

Object::ValueMap::const_iterator Object::begin() const
{
	return mValues.begin();
}

Object::ValueMap::const_iterator Object::end() const
{
	return mValues.end();
}

Object::ValueMap::iterator Object::begin()
{
	return mValues.begin();
}

Object::ValueMap::iterator Object::end()
{
	return mValues.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SerializeArray(const Array& a);

std::string SerializeValue(const Value& v)
{
	std::string str;

	switch (v.GetType())
	{
		case IntVal			: str = std::to_string((int)v); break;
		case FloatVal		: str = std::to_string((float)v); break;
		case BoolVal		: str = v ? "true" : "false"; break;
		case NULLVal		: str = "null"; break;
		case ObjectVal		: str = Serialize(v); break;
		case ArrayVal		: str = SerializeArray(v); break;
		case StringVal		: str = std::string("\"") + (std::string)v + std::string("\""); break;
	}

	return str;
}

std::string SerializeArray(const Array& a)
{
	std::string str = "[";

	bool first = true;
	for (auto& v : a)
	{
		if (!first)
			str += std::string(",");

		str += SerializeValue(v);

		first = false;
	}

	str += "]";
	return str;
}

std::string json::Serialize(const Object& obj)
{
	std::string str = "{";

	bool first = true;
	for (const auto& kv : obj)
	{
		if (!first)
			str += std::string(",");

		str += std::string("\"") + kv.first + std::string("\":") + SerializeValue(kv.second);
		first = false;
	}

	str += "}";
	return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Array DeserializeArray(std::string& str);

Value DeserializeValue(std::string& str)
{
	Value v;

	if (str.length() == 0)
		return v;

	if (str[0] == '[')
	{
		int depth = 1;
		size_t i = 1;
		for (; i < str.length(); i++)
		{
			if (str[i] == '[')
				++depth;
			else if ((str[i] == ']') && (--depth == 0))
				break;
		}

		assert(depth == 0);
		if (depth != 0)
			return v;

		std::string array_str = str.substr(0, i + 1);
		v = Value(DeserializeArray(array_str));
		str = str.substr(i + 1, str.length());
	}
	else if (str[0] == '{')
	{
		int depth = 1;
		size_t i = 1;
		for (; i < str.length(); i++)
		{
			if (str[i] == '{')
				++depth;
			else if ((str[i] == '}') && (--depth == 0))
				break;
		}

		assert(depth == 0);
		if (depth != 0)
			return v;

		std::string obj_str = str.substr(0, i + 1);
		v = Value(Deserialize(obj_str));
		str = str.substr(i + 1, str.length());
	}
	else if (str[0] == '\"')
	{
		size_t end_quote = str.find('\"', 1);
		assert(end_quote != std::string::npos);
		if (end_quote == -1)
			return v;

		v = Value(str.substr(1, end_quote - 1));
		str = str.substr(end_quote + 1, str.length());
	}
	else
	{
		bool has_dot = false;
		std::string temp_val;
		size_t i = 0;
		for (; i < str.length(); i++)
		{
			if (str[i] == '.')
				has_dot = true;
			else if ((str[i] == ']') || (str[i] == '}') || (str[i] == ','))
				break;

			temp_val += str[i];
		}

		if (has_dot)
			v = Value((float)atof(temp_val.c_str()));
		else if (_stricmp(temp_val.c_str(), "true") == 0)
			v = Value(true);
		else if (_stricmp(temp_val.c_str(), "false") == 0)
			v = Value(false);
		else if (_stricmp(temp_val.c_str(), "null") == 0)
			v = Value();
		else
			v = Value(atoi(temp_val.c_str()));

		str = str.substr(i, str.length());
	}

	return v;
}

Array DeserializeArray(std::string& str)
{
	Array a;

	assert((str[0] == '[') && (str[str.length() - 1] == ']'));

	if ((str[0] == '[') && (str[str.length() - 1] == ']'))
		str = str.substr(1, str.length() - 2);
	else
		return a;

	while (str.length() > 0)
	{
		std::string tmp;

		size_t i = 0;
		for (; i < str.length(); i++)
		{
			if ((str[i] == '{') || (str[i] == '['))
			{
				Value v = DeserializeValue(str);
				if (v.GetType() != NULLVal)
					a.push_back(v);

				break;
			}

			bool terminate_parsing = false;

			if ((str[i] == ',') || (str[i] == ']'))
				terminate_parsing = true;
			else
			{
				tmp += str[i];
				if  (i == str.length() - 1)
					terminate_parsing = true;
			}

			if (terminate_parsing)
			{
				Value v = DeserializeValue(tmp);
				if (v.GetType() != NULLVal)
					a.push_back(v);

				str = str.substr(i + 1, str.length());
				break;
			}
		}
	}

	return a;
}

Object json::Deserialize(std::string& str)
{
	Object obj;

	assert((str[0] == '{') && (str[str.length() - 1] == '}'));
	if ((str[0] == '{') && (str[str.length() - 1] == '}'))
		str = str.substr(1, str.length() - 2);
	else
		return obj;

	while (str.length() > 0)
	{
		// Get the key name
		size_t start_quote_idx = str.find('\"');
		size_t end_quote_idx = str.find('\"', start_quote_idx + 1);
		size_t colon_idx = str.find(':', end_quote_idx);

		assert((start_quote_idx != std::string::npos) && (end_quote_idx != std::string::npos) && (colon_idx != std::string::npos));
		std::string key = str.substr(start_quote_idx + 1, end_quote_idx - start_quote_idx - 1);
		assert(key.length() > 0);
		if (key.length() == 0)
			return Object();

		str = str.substr(colon_idx + 1, str.length());
		obj[key] = DeserializeValue(str);
	}

	return obj;
}