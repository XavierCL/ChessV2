#pragma once

#include "Option.hpp"
#include "FastMath.hpp"

#include <utility>
#include <iostream>

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
class FixedUnorderedMap
{
public:
	FixedUnorderedMap(const size_t& minCapacity)
		: _capacity(FastMath::nextOrSamePrime(minCapacity)),
		_size(0),
		_container(new Option<std::pair<_KeyType, _ValueType>>[_capacity])
	{}

	const Option<_ValueType> get(const _KeyType& key) const
	{
		return _container[_hasher(key) % _capacity].flatMap<_ValueType>([&key](const std::pair<_KeyType, _ValueType>& element) {
			if (element.first == key)
			{
				return Option<_ValueType>(element.second);
			}
			else
			{
				return Option<_ValueType>();
			}
		});
	}

	void set(const _KeyType& key, const _ValueType& value)
	{
		_container[_hasher(key) % _capacity] = Option<std::pair<_KeyType, _ValueType>>(std::make_pair(key, value));
	}

	void remove(const _KeyType& key)
	{
		const size_t keyIndex = _hasher(key) % _capacity;
		return _container[keyIndex].foreach([&key, this, &keyIndex](const std::pair<_KeyType, _ValueType>& element) {
			if (element.first == key)
			{
				_container[keyIndex] = Option<std::pair<_KeyType, _ValueType>>();
			}
		});
	}

	void clear()
	{
		for (size_t i = 0; i < _capacity; ++i)
		{
			_container[i] = Option<std::pair<_KeyType, _ValueType>>();
		}
	}

	~FixedUnorderedMap()
	{
		delete[] _container;
	}

private:
	const size_t _capacity;
	Option<std::pair<_KeyType, _ValueType>>* const _container;
	const _KeyHashType _hasher;
	size_t _size;
};