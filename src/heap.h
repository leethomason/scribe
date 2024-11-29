#pragma once

#include "error.h"
#include "type.h"

#include <vector>

/* 
* Construction sets the ref count to 0 - normally incremented when attached to a heap pointer
*/
class HeapObject {
public:
	HeapObject() {}
	virtual ~HeapObject() {}


	void addRef() {
		_refCount++;
	}
	void release() {
		--_refCount;
		REQUIRE(_refCount >= 0);
	}
	int getRefCount() const {
		return _refCount;
	}

private:
	int _refCount = 0;
};

class Heap {
public:
	void add(HeapObject* obj) {
		_objects.push_back(obj);
		_activeObjects++;
	};

	void release(HeapObject* obj) {
		obj->release();
		if (obj->getRefCount() == 0) {
			_activeObjects--;
		}
		if (_activeObjects == 0 || _activeObjects < _objects.size() / 2) {
			collect();
		}
	}

	// This is not garbage collection, just a way to clean up the heap.
	// Objects can be using memory with no references, so this collect()
	// method will delete them.
	void collect();

private:
	std::vector<HeapObject*> _objects;
	int _activeObjects = 0;
};

// A heap pointer is a smart pointer that automatically increments the ref count
// List* list = new List();
// HeapPtr<List> listPtr(heap, list);
//
class HeapPtr {
public:
	HeapPtr() : _heap(nullptr), _ptr(nullptr) {}
	HeapPtr(Heap* heap, HeapObject* ptr) : _heap(heap), _ptr(ptr) {
		REQUIRE(heap);
		REQUIRE(ptr);
		_heap->add(_ptr);
		_ptr->addRef();
	}
	HeapPtr(const HeapPtr& other) : _heap(other._heap), _ptr(other._ptr) {
		_heap->add(_ptr);
		_ptr->addRef();
	}
	~HeapPtr() {
		_heap->release(_ptr);
	}
	void init(Heap* heap, HeapObject* ptr) {
		REQUIRE(!_heap);
		REQUIRE(!_ptr);
		_heap = heap;
		_ptr = ptr;
		_heap->add(_ptr);
		_ptr->addRef();
	}
private:
	Heap* _heap;
	HeapObject* _ptr;
};

class MemBuf {
public:
	MemBuf(int initial);
	~MemBuf();

	void* data();
	const void* constData() const;
	void ensureCap(int cap);
};

class List : public HeapObject {
public:
	List(ValueType valueType, int initialCap);
	~List() {}

	union PrimitiveValue {
		double vNum;
		bool vBool;
		std::string* vStr;
	};

	PrimitiveValue get(int i) const {
		REQUIRE(i >= 0 && i < _size);
		const void* data = _buf.constData();
		const char* p = (const char*)data + i * _stride;
		return *(PrimitiveValue*)p;
	}

	void set(int i, PrimitiveValue value) {
		REQUIRE(i >= 0 && i < _size);
		void* data = _buf.data();
		char* p = (char*)data + i * _stride;
		*(PrimitiveValue*)p = value;
	}

	int size() const {
		return _size;
	}

	void setSize(int size) {
		_size = size;
		_buf.ensureCap(size * _stride);
	}

private:
	ValueType _valueType;
	int _stride = 0;
	int _size = 0;
	MemBuf _buf;
};
