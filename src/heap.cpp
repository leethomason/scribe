#include "heap.h"

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
