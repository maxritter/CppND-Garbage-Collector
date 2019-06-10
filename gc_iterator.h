#pragma once

#include <iostream>

/* Exception thrown when an attempt is made to
   use an iter that exceeds the range of the
   underlying object.*/
class out_of_range_exc final : public std::exception
{
public:
	out_of_range_exc() : exception() {}
	explicit out_of_range_exc(const char* const& msg)
		: exception(msg)
	{ }
};

/* An iterator-like class for cycling through arrays
   that are pointed to by GCPtrs. Iter pointers
   do not participate in or affect garbage
   collection. Thus, an Iter pointing to
   some object does not prevent that object
   from being recycled */
template <class T>
class iter
{
	/* Current pointer value */
	T* ptr_;

	/* Points to element one past end */
	T* end_;

	/* Points to start of allocated array */
	T* begin_;

	/* Length of sequence */
	unsigned length_;

public:

	/* Default constructor */
	iter()
	{
		ptr_ = end_ = begin_ = nullptr;
		length_ = 0;
	}

	/* Construct with  known elements */
	iter(T* p, T* first, T* last)
	{
		ptr_ = p;
		end_ = last;
		begin_ = first;
		length_ = last - first;
	}

	/* Return length of sequence to which this iter points */
	unsigned size() const { return length_; }

	/* Return value pointed to by ptr, do not allow out-of-bounds access */
	T& operator*()
	{
		if ((ptr_ >= end_) || (ptr_ < begin_))
		{
			throw out_of_range_exc();
		}

		return *ptr_;
	}

	/* Return address contained in ptr, do not allow out-of-bounds access */
	T* operator->()
	{
		if ((ptr_ >= end_) || (ptr_ < begin_))
		{
			throw out_of_range_exc();
		}

		return ptr_;
	}

	/* Overloaded prefix ++ */
	iter operator++()
	{
		++ptr_;

		return *this;
	}
	/* Overloaded prefix -- */
	iter operator--()
	{
		--ptr_;

		return *this;
	}

	/* Overloaded postfix ++ */
	iter operator++(int notused)
	{
		T* tmp = ptr_;
		++ptr_;

		return iter<T>(tmp, begin_, end_);
	}

	/* Overloaded postfix -- */
	iter operator--(int notused)
	{
		T* tmp = ptr_;
		--ptr_;

		return iter<T>(tmp, begin_, end_);
	}

	/* Return a reference to the object at the specified index.
	   Do not allow out-of-bounds access */
	T& operator[](int i)
	{
		if ((i < 0) || (i >= (end_ - begin_)))
		{
			throw out_of_range_exc();
		}

		return ptr_[i];
	}

	/* Define the relational operators */
	bool operator==(iter op2)
	{
		return ptr_ == op2.ptr_;
	}

	bool operator!=(iter op2)
	{
		return ptr_ != op2.ptr_;
	}

	bool operator<(iter op2)
	{
		return ptr_ < op2.ptr_;
	}

	bool operator<=(iter op2)
	{
		return ptr_ <= op2.ptr_;
	}

	bool operator>(iter op2)
	{
		return ptr_ > op2.ptr_;
	}

	bool operator>=(iter op2)
	{
		return ptr_ >= op2.ptr_;
	}

	/* Subtract an integer from an iter */
	iter operator-(int n)
	{
		ptr_ -= n;

		return *this;
	}

	/* Add an integer to an iter */
	iter operator+(int n)
	{
		ptr_ += n;

		return *this;
	}

	/* Return number of elements between two iters */
	int operator-(iter<T>& itr2)
	{
		return ptr_ - itr2.ptr_;
	}
};
