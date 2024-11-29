#pragma once

#include "error.h"

#include <vector>

class NumList;
class BoolList;
class StrList;

/* 
* A HeapObject is constructed like any other object.
* However, it MUST:
* - be added to the heap
* - be held by a HeapPtr
*/
class HeapObject {
public:
	HeapObject() {}
	virtual ~HeapObject() {}
	virtual const char* name() const = 0;	// debugging

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
	// This is not garbage collection, just a way to clean up the heap.
	// Objects can be using memory with no references, so this collect()
	// method will delete them.
	void collect();

	// When a HeapObject is created, it MUST be added to the heap.
	void add(HeapObject* obj) {
		_objects.push_back(obj);
	};

	const std::vector<HeapObject*>& objects() const {
		return _objects;
	}

	// Reports open objects.
	// Generally want to collect() first
	void report();

private:
	std::vector<HeapObject*> _objects;
};

// A heap pointer is a smart pointer that automatically increments the ref count
class HeapPtr {
public:
	HeapPtr() : _ptr(nullptr) {}
	HeapPtr(HeapObject* ptr) : _ptr(ptr) {
		REQUIRE(ptr);
		_ptr->addRef();
	}
	HeapPtr(const HeapPtr& other) : _ptr(other._ptr) {
		if (_ptr)
			_ptr->addRef();
	}
	~HeapPtr() {
		if (_ptr) {
			_ptr->release();
		}
	}
	void set(HeapObject* ptr) {
		REQUIRE(!_ptr);
		_ptr = ptr;
		_ptr->addRef();
	}

	void clear() {
		if (_ptr) {
			_ptr->release();
			_ptr = nullptr;
		}
	}

private:
	HeapObject* _ptr;
};

template<typename T> 
class ObjList : public HeapObject
{
public:
	ObjList(int initialSize) {
		setSize(initialSize);
	}
	virtual ~ObjList() {}

	T get(int i) const {
		return _list[i];
	}

	void set(int i, T v) {
		_list[i] = v;
	}

	int size() const {
		return (int)_list.size();
	}

	void setSize(int size) {
		_list.resize(size);
	}

private:
	std::vector<double> _list;
};

class NumList : public ObjList<double> {
public:
	NumList(int initialSize) : ObjList(initialSize) {}
	virtual const char* name() const override { return "NumList"; }
};

class BoolList : public ObjList<bool> {
public:
	BoolList(int initialSize) : ObjList(initialSize) {}
	virtual const char* name() const override { return "BoolList"; }
};

class StrList : public ObjList<std::string> {
	public:
	StrList(int initialSize) : ObjList(initialSize) {}
	virtual const char* name() const override { return "StrList"; }
};

