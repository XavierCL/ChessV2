#pragma once

#include <deque>

template <typename ElementType>
class Deleter
{
public:
	void scheduleDeletion(ElementType* element)
	{
		_toBeDeleted.push_back(element);
	}

	void deleteAll()
	{
		for (auto* const& element : _toBeDeleted)
		{
			delete element;
		}
		_toBeDeleted.clear();
	}
private:
	std::deque<ElementType*> _toBeDeleted;
};