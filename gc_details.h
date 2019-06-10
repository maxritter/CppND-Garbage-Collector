#pragma once

/* This class defines an element that is stored
   in the garbage collection information list */

template <class T> class ptr_details
{

public:

	/* Current reference count */
	unsigned refcount;

	/* Pointer to allocated memory */
	T* mem_ptr;

	/* Is true if mem_ptr points to an allocated array. It is false otherwise */
	bool is_array;

	/* If mem_ptr is pointing to an allocated array, then arraySize contains its size */
	unsigned array_size = 0;

	/* Here, mPtr points to the allocated memory.
	   If this is an array, then size specifies the size of the array */
	explicit ptr_details(T* m_ptr, const unsigned size)
	{
		mem_ptr = m_ptr;
		refcount = 1;
		
		if (size != 0)
		{
			is_array = true;
			array_size = size;
		}
			
		else
		{
			is_array = false;
			array_size = 0;
		}
	}
};

/* Overloading operator== allows two class objects to be compared.
   This is needed by the STL list class */
template <class T> bool operator==(const ptr_details<T>& ob1, const ptr_details<T>& ob2)
{
	return (ob1.mem_ptr == ob2.mem_ptr);
}