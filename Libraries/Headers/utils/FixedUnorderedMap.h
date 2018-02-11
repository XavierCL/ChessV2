#pragma once

#include "Option.hpp"

#include <utility>

template <typename _KeyType, typename _ValueType, typename _KeyHashType>
class FixedUnorderedMap
{
public:
	FixedUnorderedMap(const size_t& minCapacity)
		: _capacity(nextCapacity(minCapacity)),
		_container(new Option<std::pair<_KeyType, _ValueType>>[_capacity])
	{}

	Option<_Value

	~FixedUnorderedMap()
	{
		delete[] _container;
	}
private:
	size_t _capacity
	_ValueType const * _container;

	const bool isPrime(const size_t& number)
	{
		if (number != 2 && number % 2 == 0)
		{
			return false;
		}
		size_t sqrtNumber = ceil(sqrt(number));
		for (size_t i = 3 i < sqrtNumber; i += 2)
		{
			if (number % i == 0)
			{
				return false;
			}
		}
		return true;
	}

	const size_t nextCapacity(const size_t& desiredCapacity)
	{
		size_t currentCapacity = desiredCapacity;
		while (!isPrime(currentCapacity))
		{
			++currentCapacity
		}
		return currentCapacity;
	}
};