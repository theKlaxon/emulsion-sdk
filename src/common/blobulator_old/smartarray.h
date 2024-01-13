//==== Blobulator. Copyright � 2004-2007, Rosware LLC, All rights reserved. ====//

#pragma once

#include "tier0/platform.h"

#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#define KEEP_MAX_SIZE 0

// IMPORTANT WARNING: Never keep a pointer to a smart array element.
// When the array resizes the pointer will point to junk!!!

#pragma warning( push )
#pragma warning( disable : 4127 )

template <class T, bool sse_align = false, int initialCapacity = 16> class SmartArray
{
	public:

	T* a;
	int size;
	int capacity;
#if (KEEP_MAX_SIZE == 1)
	int max_size;
#endif

	SmartArray()
	{
		size = 0;
		#if (KEEP_MAX_SIZE == 1)
			max_size = 0;
		#endif
		capacity = initialCapacity;
		if(initialCapacity > 0)
		{
			if(sse_align) a = (T*)_aligned_malloc(capacity * sizeof(T), 16);
			else a = (T*)malloc(capacity * sizeof(T));
			assert(a);
		}
		else
		{
			a = NULL;
		}
	}

	~SmartArray()
	{
		assert(initialCapacity == 0 || a);
		free(a);
	}

	/*SmartArray(int initialCapacity)
	{
		size = 0;
		capacity = initialCapacity;
		a = (T*)malloc(capacity * sizeof(T));
	}*/

	FORCEINLINE void ensureCapacity(int c)
	{
		if(capacity < c)
		{
			capacity = c * 2;
			if(sse_align)
			{	
				a = (T*)_aligned_realloc(a, capacity * sizeof(T), 16);
				// On some platforms, aligned_realloc crashes, and malloc
				// along with a memcpy must be used
				//T* temp = (T*)_aligned_malloc(capacity * sizeof(T), 16);
				//memcpy(temp, a, size * sizeof(T));
				//_aligned_free(a);
				//a = temp;
			}
			else
			{
				a = (T*)realloc(a, capacity * sizeof(T));
			}
			assert(a);
		}
	}

	// Make sure we can add c more elements to the array
	FORCEINLINE void ensureAdditionalCapacity(int c)
	{
		c += size;
		if(capacity < c)
		{
			capacity = c * 2;
			if(sse_align)
			{
				a = (T*)_aligned_realloc(a, capacity * sizeof(T), 16);
				//T* temp = (T*)_aligned_malloc(capacity * sizeof(T), 16);
				//memcpy(temp, a, size * sizeof(T));
				//_aligned_free(a);
				//a = temp;
			}
			else
			{
				a = (T*)realloc(a, capacity * sizeof(T));
			}
			assert(a);
		}
	}

	FORCEINLINE void resize(int c)
	{
		capacity = c;
		if(sse_align)
		{
			a = (T*)_aligned_realloc(a, capacity * sizeof(T), 16);
			//T* temp = (T*)_aligned_malloc(capacity * sizeof(T), 16);
			//memcpy(temp, a, size * sizeof(T));
			//_aligned_free(a);
			//a = temp;
		}
		else
		{
			a = (T*)realloc(a, capacity * sizeof(T));
		}
		assert(a);
	}

	FORCEINLINE T& operator[] (int i)
	{
		assert(i >= 0 && i < capacity);
		return a[i];
	}

	FORCEINLINE void put(int i, const T& e)
	{
		assert(i >= 0 && i < size);
		a[i] = e;
	}

	/*
	FORCEINLINE void putAutoSize(int i, const T& e)
	{
		assert(i >= 0);
		if(i >= size)
		{
			size = i + 1;
			resize(size * 2);
		}
		a[i] = e;
	}
	*/

	FORCEINLINE void push(const T& e)
	{
		assert(size < capacity);
		a[size] = e;
		size ++;
	}

	// Many times, you want to push a new item, but rather
	// than passing in data, you want to just get a pointer
	// or a reference to it so that you can then write
	// directly into that item. That's what this function is for.
	FORCEINLINE T& push()
	{
		assert(size < capacity);
		return a[size++];
	}

	FORCEINLINE void push2(const T& e1, const T& e2)
	{
		assert(size < capacity);
		a[size] = e1;
		a[size+1] = e2;
		size += 2;
	}

	FORCEINLINE void push2AutoSize(const T& e1, const T& e2)
	{
		if(size+1 >= capacity)
		{
			resize((size+2) * 2);
		}
		a[size] = e1;
		a[size+1] = e2;
		size += 2;
	}

	FORCEINLINE void pushAutoSize(const T& e)
	{
		if(size >= capacity)
		{
			resize((size + 1) * 2);
		}
		a[size] = e;
		size ++;
	}

	// This is analogous to the push() function
	FORCEINLINE T& pushAutoSize()
	{
		if(size >= capacity)
		{
			resize((size + 1) * 2);
		}
		return a[size++];
	}

	FORCEINLINE T& pop(void)
	{
		assert(size >= 1);
		return a[--size];
	}

	FORCEINLINE void pop2(T& e1, T& e2)
	{
		assert(size >= 1);
		e1 = a[size-2];
		e2 = a[size-1];
		size -= 2;
	}

	FORCEINLINE void remove(int i)
	{
		assert(i >= 0 && i < size);
		for( ;i < size-1; i++)
		{
			a[i] = a[i+1];
		}
		size--;
	}

	FORCEINLINE T removeAndReturn(int i)
	{
		assert(i >= 0 && i < size);
		T removed = a[i];
		for( ;i < size-1; i++)
		{
			a[i] = a[i+1];
		}
		size--;
		return removed;
	}

	FORCEINLINE int find(const T& e)
	{
		for(int i = 0; i < size; i++)
		{
			if(e == a[i]) return i;
		}
		return -1;
	}

	FORCEINLINE bool findAndRemove(const T& e)
	{
		int i = find(e);
		if(i == -1)
		{
			return false;
		}
		else
		{
			remove(i);
			return true;
		}
	}

	FORCEINLINE void findAndRemoveAll(const T& e)
	{
		while(findAndRemove(e) == true) {};
	}

	// Get the last item
	FORCEINLINE T& last()
	{
		return a[size-1];
	}

	template <class C>
	FORCEINLINE bool testSort()
	{
		for(int i = 0; i < size-1; i++)
		{
			if(C::gt(a[i], a[i+1])) return false;
		}
		return true;
	}

	// TODO: Should try using swap in sort
	FORCEINLINE void swap(int i, int j)
	{
		T t = a[i]; a[i] = a[j]; a[j] = t;
	}

	// The following function quicksorts the smart array.
	// It uses a median of the first, last, and middle value of a
	// subarray to find pivots, thus it does well on both sorted,
	// and reverse sorted arrays. It also does well on arrays that
	// contain many duplicate values.
	template <class C>
	FORCEINLINE void sort()
	{
		if(size == 2)
		{
			if(C::gt(a[0], a[1])) { T t = a[0]; a[0] = a[1]; a[1] = t;  }
		}
		else if(size > 2)
		{
			// size is 3 or more
			sort<C>(0, size - 1);
		}
	}


	// The following function quicksorts an array of size 3 or more.
	// For identifying the pivot, it takes the median of the first, last
	// and central item. Due to this, it performs well on arrays that are
	// sorted, backwards sorted, and on degenerate cases such as an array
	// where many values are the same.

	// This code has been thoroughly tested.... do not change without
	// careful testing afterwards

	// TODO: Evaluate whether making variables static in here actually
	// saves stack space, and whether it makes the code run faster.
	// TODO: Use macros for swapping items, and for insuring an ordering

	// Note I took out statics because they weren't thread safe =)
	#define SORT_STATIC 

	template <class C>
	void sort(int lo0, int hi0)
	{
		//depth ++;
		//DevMsg("x %d %d %d\n", lo0, hi0, depth);
		//fflush(stdout);

		SORT_STATIC int c0;
		c0 = (lo0 + hi0) / 2;

		SORT_STATIC T t;

		if(C::gt(a[lo0], a[hi0])) { t = a[lo0]; a[lo0] = a[hi0]; a[hi0] = t; }
		if(C::gt(a[lo0], a[c0]))  { t = a[lo0]; a[lo0] = a[c0];  a[c0] = t;  }
		if(C::gt(a[c0],  a[hi0])) { t = a[c0];  a[c0]  = a[hi0]; a[hi0] = t; }

		// if the size is 3, everything is now sorted... return.
		// the size should not be less than 3
		if(hi0 - lo0 <= 2) return;

		//DevMsg("%d %d %d\n", a[lo0], a[c0], a[hi0]);
		
		// Find the value of the pivot and swap out the pivot to the end of the array
		// We don't explicitly need to store the pivot in the end of the array because
		// we store it inside of mid.
		// NOTE: A slight optimization might be to do the sort above for size==3 and to
		// do a slightly different sort here which doesn't modify a[hi0] (since it gets
		// overwritten with a[lo] after the partition step anyway.
		SORT_STATIC T mid;
		mid = a[c0]; a[c0] = a[hi0]; /*a[hi0] = mid;*/

		// lo is set to lo0-1 instead of lo0 and hi is set to hi0 instead
		// of hi0-1 because in the while loop, we immediately add +1 to lo
		// and subtract 1 from hi
		int lo = lo0-1; // lo can be made static
		int hi = hi0;

		// This is the partition step
		assert(lo < hi);
		while (1)
		{
			// Note that because of the 3 way sort at the beginning, we are guaranteed
			// that there is either one value greater than or equal to the pivot
			// and that there is another value less than or equal to the pivot between lo0 and hi0-1.
			// Thus, there is no way that lo can go below lo0 and that hi can go above hi0-1.

			// We use a do-while loop here instead of a regular while loop, because
			// we always want lo to be incremented and hi to be decremented at the start
			// of each iteration. Otherwise, we can go into an infinite loop if there are
			// multiple items equal to the pivot.

			do {lo++;} while (C::gt(mid, a[lo]));
			do {hi--;} while (C::gt(a[hi], mid));

			if (lo >= hi) break;
			  
			t = a[lo]; a[lo] = a[hi]; a[hi] = t;
		}

		assert(hi >= lo0);
		assert(lo <= hi0-1);

		// Move the mid back to the middle. We swap mid with lo because
		// we are guaranteed that lo ends up at a value that is greater or
		// equal to mid, thus we want that value to go to the end of the array
		// rather than sending a[hi] to the end (which could be less than mid).
		a[hi0] = a[lo];
		a[lo] = mid;

		// Update lo to be the element before mid, and hi to be the element
		// after mid
		hi = lo + 1;
		lo = lo - 1;

		// The two subarrays to be sorted now are lo0...lo, hi...hi0

		SORT_STATIC int lSizeM1; lSizeM1 = lo-lo0; // the size of the left subarray minus one
		// If the left subarray is size 2, sort it here.
		// If it is size 3 or more, recurse.
		// If it is size 1, do nothing.
		if(lSizeM1 == 1)
		{
			if(C::gt(a[lo0], a[lo])) { t = a[lo0]; a[lo0] = a[lo]; a[lo] = t; }
		}
		else if(lSizeM1 > 1)
		{
			sort<C>(lo0, lo);
		}

		SORT_STATIC int rSizeM1; rSizeM1 = hi0-hi; // the size of the right subarray minus one
		// If the right subarray is size 2, sort it here.
		// If it is size 3 or more, recurse.
		// If it is size 1 do nothing.
		if(rSizeM1 == 1)
		{
			if(C::gt(a[hi], a[hi0])) { t = a[hi]; a[hi] = a[hi0]; a[hi0] = t; }
		}
		else if(rSizeM1 > 1)
		{
			sort<C>(hi, hi0);
		}

		//depth --;
	}


	FORCEINLINE void shuffle(int first = INT_MIN, int last = INT_MIN)
	{
		if(first == INT_MIN && last == INT_MIN) { first = 0; last = size-1; }

		if(last <= first) return;

		//int first = 0;
		//int last;
		//if(_size == -1) last = size - 1;
		//else last = _size - 1;

		const int _RANDOM_BITS = 15;	// minimum random bits from rand()
		const int _RANDOM_MAX = (1U << _RANDOM_BITS) - 1;

		int next = first;

		for(unsigned long index = 2; ++next != last; ++index)
		{
			// assume unsigned long big enough for _Diff count
			unsigned long Rm = _RANDOM_MAX;
			unsigned long Rn = ::rand() & _RANDOM_MAX;
			int iters = 0;
			for (; Rm < index && Rm != ~0UL; Rm = (Rm << _RANDOM_BITS) | _RANDOM_MAX)
			{
				Rn = (Rn << _RANDOM_BITS) | ::rand();	// build random value
				iters ++;
			}
			if(iters > 1)
			{
				DevMsg("Iters greater than 1\n");
			}

			// Swap the next item on the list with any of the preceeding items, including itself.
			// Does this mean that items close to the beginning are more likely to stay at the beginning?
			int diff = Rn % index; // index is 1+next if first = 0
			int swap_with = first + diff;

			T temp = a[next];
			a[next] = a[swap_with];
			a[swap_with] = temp;
		}
	}

	FORCEINLINE void swapData(SmartArray<T, sse_align, initialCapacity>& other)
	{
		T* tempa = a;
		a = other.a;
		other.a = tempa;

		int temp_size = size;
		size = other.size;
		other.size = temp_size;
		
		#if (KEEP_MAX_SIZE == 1)
			int temp_max_size = max_size;
			max_size = other.max_size;
			other.max_size = temp_max_size;
		#endif
			
		int temp_capacity = capacity;
		capacity = other.capacity;
		other.capacity = temp_capacity;
	}
};

#pragma warning( pop )