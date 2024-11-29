#include "heap.h"

#include <fmt/core.h>

void Heap::collect()
{
	// delete unreferenced objects
	for (size_t i = 0; i < _objects.size(); i++) {
		if (_objects[i]->getRefCount() == 0) {
			delete _objects[i];
			_objects[i] = nullptr;
		}
	}
	_objects.erase(std::remove(_objects.begin(), _objects.end(), nullptr), _objects.end());
}

void Heap::report()
{
	fmt::print("Heap has {} objects\n", _objects.size());
	for (const auto& obj : _objects) {
		fmt::print("  Object {} has {} refs\n", obj->name(), obj->getRefCount());
	}
}