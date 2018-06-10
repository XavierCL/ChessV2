#pragma once

#include <cstdint>

template <typename _Type>
class PointerHash
{
public:

	const size_t operator()(_Type const * const &hashed) const
	{
		return reinterpret_cast<uintptr_t>(hashed) % static_cast<uintptr_t>(sizeof(size_t));
	}
};