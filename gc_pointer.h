#pragma once

#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

/*
	Pointer implements a pointer type that uses
	garbage collection to release unused memory.
	A Pointer must only be used to point to memory
	that was dynamically allocated using new.
	When used to refer to an allocated array,
	specify the array size.
*/

template <class T, int Size = 0> class pointer
{

	/* Maintains the garbage collection list */
	static std::list<ptr_details<T> > ref_container_;

	/* Points to the allocated memory to which this Pointer pointer currently points */
	T* addr_;

	/* Is true if this Pointer points to an allocated array, false otherwise */
	bool is_array_;

	/* If this Pointer is pointing to an allocated array, then contains its size */
	unsigned array_size_;

	/* True when first Pointer is created */
	static bool first_;

	/* Return an iterator to pointer details in refContainer */
	typename std::list<ptr_details<T> >::iterator find_ptr_info(T* ptr);

public:

	/* Define an iterator type for Pointer<T> */
	typedef iter<T> gc_iterator;

	pointer() : addr_(nullptr), is_array_(false), array_size_(0)
	{
		pointer(NULL);
	}

	pointer(T*);

	/* Copy constructor */
	pointer(const pointer&);

	/* Destructor */
	~pointer();

	/* Collect garbage. Returns true if at least one object was freed */
	static bool collect();

	/* Overload assignment of pointer to Pointer */
	T* operator=(T* t);

	/* Overload assignment of Pointer to Pointer */
	pointer& operator=(pointer& rv);

	/* Return a reference to the object pointed to by this Pointer */
	T& operator*()
	{
		return *addr_;
	}

	/* Return the address being pointed to */
	T* operator->()
	{
		return addr_;
	}

	/* Return a reference to the object at the index specified by i */
	T& operator[](int i)
	{
		return addr_[i];
	}

	/* Conversion function to T* */
	explicit operator T* () {
		return addr_;
	}

	/* Return an iter to the start of the allocated memory */
	iter<T> begin()
	{
		int size;
		if (is_array_)
		{
			size = array_size_;
		}

		else
		{
			size = 1;
		}

		return iter<T>(addr_, addr_, addr_ + size);
	}

	/* Return an iter to one past the end of an allocated array */
	iter<T> end()
	{
		int size;
		if (is_array_)
		{
			size = array_size_;
		}

		else
		{
			size = 1;
		}

		return iter<T>(addr_ + size, addr_, addr_ + size);
	}

	/* Return the size of refContainer for this type of Pointer */
	static size_t ref_container_size()
	{
		return ref_container_.size();
	}

	/* An utility function that displays ref_container */
	static void showlist();

	/* Clear ref_container when program exits */
	static void shutdown();
};

/* STATIC INITIALIZATION */

/* Creates storage for the static variables */
template <class T, int Size>
std::list<ptr_details<T> > pointer<T, Size>::ref_container_;
template <class T, int Size>
bool pointer<T, Size>::first_ = true;

/* Constructor for both initialized and uninitialized objects */
template<class T, int Size> pointer<T, Size>::pointer(T* t)
{
	/* Register shutdown() as an exit function */
	if (first_)
	{
		atexit(shutdown);
	}
	first_ = false;

	auto p = find_ptr_info(t);

	/* Already in the list, increment reference counter */
	if (p != ref_container_.end())
	{
		++p->refcount;
	}

	/* Add the entry to the list */
	else
	{
		ptr_details<T> pointer_obj(t, Size);
		ref_container_.push_front(pointer_obj);
	}

	addr_ = t;
	array_size_ = Size;

	if (array_size_ > 0)
	{
		std::cout << " Constructed a new pointer array with size: " << array_size_ << std::endl;
		is_array_ = true;
	}

	else
	{
		std::cout << "Constructed a new pointer object" << std::endl;
		is_array_ = false;
	}
}

/* Copy constructor */
template< class T, int Size> pointer<T, Size>::pointer(const pointer& ob)
{
	auto p = find_ptr_info(ob.addr_);

	/* Increment reference count and copy information */
	++p->refcount;
	addr_ = ob.addr_;
	array_size_ = ob.array_size_;

	if (array_size_ > 0)
	{
		std::cout << " Copy-Constructed a new pointer array with size: " << array_size_ << std::endl;
		is_array_ = true;
	}

	else
	{
		std::cout << "Copy-Constructed a new pointer object" << std::endl;
		is_array_ = false;
	}
}

/* Destructor for Pointer */
template <class T, int Size> pointer<T, Size>::~pointer()
{
	auto p = find_ptr_info(addr_);

	/* Pointer out of scope, decrement reference count */
	if (p->refcount)
	{
		--p->refcount;
	}
	std::cout << "Pointer destructed, decrementing reference counter" << std::endl;

	/* Eventually collect garbage */
	collect();
}

/* Collect garbage,  returns true if at least one object was freed */
template <class T, int Size> bool pointer<T, Size>::collect()
{
	auto mem_freed = false;

	/* Print some information about the list before garbage collection */
	std::cout << "\nBefore garbage collection for ";
	showlist();

	typename std::list<ptr_details<T> >::iterator p;
	do
	{
		/* Scan our garbage container list for unreferenced items */
		for (p = ref_container_.begin(); p != ref_container_.end(); ++p)
		{
			/* Pointer still in-use, continue with next */
			if (p->refcount > 0)
			{
				continue;
			}

			/* Otherwise free it */
			mem_freed = true;
			ref_container_.remove(*p);
			if (p->mem_ptr)
			{
				/* Delete array */
				if (p->is_array)
				{
					std::cout << "Deleting array of size " << p->array_size << std::endl;
					delete[] p->mem_ptr;
				}

				/* Delete single object */
				else
				{

					std::cout << "Deleting: " << *static_cast<T*>(p->mem_ptr) << std::endl;
					delete p->mem_ptr;
				}
			}

			/* Do the search all again */
			break;
		}
	} while (p != ref_container_.end());

	/* Print some information about the list after garbage collection */
	std::cout << "\nAfter garbage collection for ";
	showlist();

	return mem_freed;
}

// Overload assignment of pointer to Pointer.
template <class T, int Size> T* pointer<T, Size>::operator=(T* t)
{
	auto p = find_ptr_info(addr_);

	/* Decrement the reference count for the memory currently being pointed to */
	--p->refcount;

	p = find_ptr_info(t);
	/* Already in the list, increment reference counter */
	if (p != ref_container_.end())
	{
		++p->refcount;
	}

	/* Add the entry to the list */
	else
	{
		ptr_details<T> pointer_obj(t, Size);
		ref_container_.push_front(pointer_obj);
	}

	/* Store the address */
	addr_ = t;

	return t;
}

/* Overload assignment of Pointer to Pointer */
template <class T, int Size> pointer<T, Size>& pointer<T, Size>::operator=(pointer& rv)
{
	auto p = find_ptr_info(addr_);

	/* Decrement the reference count for the memory currently being pointed to */
	--p->refcount;

	/* Increment the reference count of the new address */
	p = find_ptr_info(rv.addr_);
	++p->refcount;

	/* Store the address */
	addr_ = rv.addr_;

	return rv;
}

/* An utility function that displays ref_container */
template <class T, int Size> void pointer<T, Size>::showlist()
{
	std::cout << "refContainer<" << typeid(T).name() << ", " << Size << ">:\n";
	std::cout << "mem_ptr ref count value: " << std::endl;

	if (ref_container_.begin() == ref_container_.end())
	{
		std::cout << " Container is empty!\n\n ";
	}

	else
	{
		for (auto p = ref_container_.begin(); p != ref_container_.end(); ++p)
		{
			std::cout << "[" << static_cast<void*>(p->mem_ptr) << "]" << " " << p->refcount << " ";
			if (p->mem_ptr)
			{
				std::cout << " " << *p->mem_ptr;
			}

			else
			{
				std::cout << "---";
			}

			std::cout << std::endl;
		}
	}
	
	std::cout << std::endl;
}

/* Find a pointer in ref_container */
template <class T, int Size> typename std::list<ptr_details<T> >::iterator pointer<T, Size>::find_ptr_info(T* ptr)
{
	typename std::list<ptr_details<T> >::iterator p;

	/* Find ptr in refContainer */
	for (p = ref_container_.begin(); p != ref_container_.end(); ++p)
	{
		if (p->mem_ptr == ptr)
		{
			return p;
		}
	}

	return p;
}

/* Clear ref_container, when program exits */
template <class T, int Size> void pointer<T, Size>::shutdown()
{
	/* Empty list */
	if (ref_container_size() == 0)
	{
		return;
	}

	/* Set all reference counts to zero */
	for (auto p = ref_container_.begin(); p != ref_container_.end(); ++p)
	{
		p->refcount = 0;
	}

	/* Run our garbage collector */
	collect();
}