#pragma once

#include "Option.hpp"
#include "FastMath.hpp"

#include <iostream>
#include <utility>

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
class FixedUnorderedMap
{
public:
	FixedUnorderedMap(const size_t& minCapacity)
		: _capacity(FastMath::nextOrSamePrime(minCapacity)),
		_container(new Option<std::pair<_KeyType, _ValueType>>[_capacity])
	{}

	const Option<_ValueType> get(const _KeyType& key) const
	{
		return _container[_hasher(key) % _capacity].flatMap<_ValueType>([&key](const std::pair<_KeyType, _ValueType>& element) {
			if (element.first == key)
			{
				++cacheHit;
				return Option<_ValueType>(element.second);
			}
			else
			{
				++dualHash;
				return Option<_ValueType>();
			}
		});
	}

	void set(const _KeyType& key, const _ValueType& value)
	{
		size_t index = _hasher(key) % _capacity;
		if (!_container[index].isDefined())
		{
			++size;
		}
		++cacheMiss;
		if (cacheMiss % 100000 == 0)
		{
			std::cout << "size: " << size << "\n";
			std::cout << "cache hit: " << cacheHit << "\n";
			std::cout << "cache Miss: " << cacheMiss << "\n";
			std::cout << "dual Hash: " << dualHash << "\n";
			std::cout << "empty set: " << cacheMiss - dualHash << "\n";
			std::cout << "sample: " << _hasher(key) << "\n\n";
		}
		_container[index] = Option<std::pair<_KeyType, _ValueType>>(std::make_pair(key, value));
	}

	~FixedUnorderedMap()
	{
		delete[] _container;
	}

private:
	const size_t _capacity;
	Option<std::pair<_KeyType, _ValueType>>* const _container;
	const _KeyHashType _hasher;

	static size_t size;
	static size_t cacheHit;
	static size_t cacheMiss;
	static size_t dualHash;
};

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
size_t FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType>::size = 0;

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
size_t FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType>::cacheHit = 0;

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
size_t FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType>::cacheMiss = 0;

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
size_t FixedUnorderedMap<_KeyType, _ValueType, _KeyHashType>::dualHash = 0;