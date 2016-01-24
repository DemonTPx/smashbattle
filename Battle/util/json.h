#pragma once
#include <vector>
#include <map>
#include <string>

// See README.txt for usage and other info.

namespace json
{
	enum ValueType
	{
		NULLVal,
		StringVal,
		IntVal,
		FloatVal,
		ObjectVal,
		ArrayVal,
		BoolVal
	};

	class Value;

	class Object
	{
		protected:

			typedef std::map<std::string, Value> ValueMap;
			ValueMap	mValues;

		public:

			Object();
			Object(const Object& obj);
			Object& operator =(const Object& obj);
			Value& operator [](const std::string& key);

			ValueMap::const_iterator begin() const;
			ValueMap::const_iterator end() const;
			ValueMap::iterator begin();
			ValueMap::iterator end();

			size_t size() const {return mValues.size();}

	};

	class Array
	{
		protected:

			typedef std::vector<Value> ValueVector;
			ValueVector				mValues;

		public:

			Array();
			Array(const Array& a);
			Array& operator=(const Array& a);

			Value& operator[] (size_t i);

			ValueVector::const_iterator begin() const;
			ValueVector::const_iterator end() const;
			ValueVector::iterator begin();
			ValueVector::iterator end();

			void push_back(const Value& v);
			void insert(size_t index, const Value& v);
			size_t size() const;
	};

	class Value
	{
		protected:

			ValueType						mValueType;
			int								mIntVal;
			float							mFloatVal;
			std::string						mStringVal;
			Object							mObjectVal;
			Array							mArrayVal;
			bool 							mBoolVal;

		public:

			Value() 					: mValueType(NULLVal) {}
			Value(const int v)			: mValueType(IntVal), mIntVal(v) {}
			Value(const float v)		: mValueType(FloatVal), mFloatVal(v) {}
			Value(const std::string& v)	: mValueType(StringVal), mStringVal(v) {}
			Value(const char* v)		: mValueType(StringVal), mStringVal(v) {}
			Value(const Object& v)		: mValueType(ObjectVal), mObjectVal(v) {}
			Value(const Array& v)		: mValueType(ArrayVal), mArrayVal(v) {}
			Value(const bool v)			: mValueType(BoolVal), mBoolVal(v) {}
			Value(const Value& v);

			ValueType GetType() const {return mValueType;}

			Value& operator =(const Value& v);
			operator int() const 			{return mIntVal;}
			operator float() const 			{return mFloatVal;}
			operator bool() const 			{return mBoolVal;}
			operator std::string() const 	{return mStringVal;}
			operator Object() const 		{return mObjectVal;}
			operator Array() const 			{return mArrayVal;}
			Value& operator [](size_t idx);
			Value& operator [](const std::string& key);

	};

	// Converts the Object to a JSON string. Will assert on error.
	std::string Serialize(const Object& obj);

	// This WILL mangle the input string. Converts a JSON formatted string to an Object.
	// Will assert on error.
	Object 		Deserialize(std::string& str);
}