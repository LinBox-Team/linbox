/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/vector/bit-vector.h
 * Copyright (C) 2003 Bradford Hovinen
 *
 * -------------------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __BIT_VECTOR_H
#define __BIT_VECTOR_H

#include <iterator>
#include <vector>
#include <stdexcept>

#include "linbox/vector/vector-traits.h"

namespace LinBox
{

/** Bit vector class
 *
 * This class provides a vector of boolean 0-1 values, stored compactly to save
 * space. It provides an additional iterator, @ref{word_iterator}, that gives
 * the bits in compact 32-bit words, so that vector operations may be done in
 * parallel. It is similar to the STL bit_vector except that it provides the
 * aforementioned additional iterator.
 */

class BitVector
{
    public:
	typedef bool        value_type;
	typedef size_t      size_type;
	typedef int         difference_type;
	typedef std::vector<unsigned long>::iterator               word_iterator;
	typedef std::vector<unsigned long>::const_iterator         const_word_iterator;
	typedef std::vector<unsigned long>::reverse_iterator       reverse_word_iterator;
	typedef std::vector<unsigned long>::const_reverse_iterator const_reverse_word_iterator;

	BitVector () {}
	BitVector (std::vector<bool> &v)
		{ *this = v; }
	BitVector (std::vector<unsigned long> &v)
		: _v (v), _size (_v.size () * 32) {}
	BitVector (size_t n, bool val = false)
		{ resize (n, val); }
		
	// Copy constructor
	BitVector (const BitVector &v) 
		: _v (v._v), _size (v._size) {}

	~BitVector () {}

	// Reference

	class reference;
	class const_reference;

	// Iterators

	class iterator;
	class const_iterator;

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef iterator    pointer;

	inline iterator                    begin      (void);
	inline const_iterator              begin      (void) const;
	inline iterator                    end        (void);
	inline const_iterator              end        (void) const;

	inline reverse_iterator            rbegin     (void);
	inline const_reverse_iterator      rbegin     (void) const;
	inline reverse_iterator            rend       (void);
	inline const_reverse_iterator      rend       (void) const;

	inline word_iterator               wordBegin  (void)       { return _v.begin (); }
	inline const_word_iterator         wordBegin  (void) const { return _v.begin (); }
	inline word_iterator               wordEnd    (void)       { return _v.end (); }
	inline const_word_iterator         wordEnd    (void) const { return _v.end (); }

	inline reverse_word_iterator       wordRbegin (void)       { return _v.rbegin (); }
	inline const_reverse_word_iterator wordRbegin (void) const { return _v.rbegin (); }
	inline reverse_word_iterator       wordRend   (void)       { return _v.rend (); }
	inline const_reverse_word_iterator wordRend   (void) const { return _v.rend (); }

	// Element access

	inline reference       operator[] (size_type n);
	inline const_reference operator[] (size_type n) const;

	reference at       (size_type n);
	const_reference at (size_type n) const;

	inline reference       front (void);
	inline const_reference front (void) const;
	inline reference       back  (void);
	inline const_reference back  (void) const;

	template<class Container>
	BitVector &operator = (const Container& x);

	void resize (size_type new_size, bool val = false);

	inline size_type size      (void) const { return _size;            }
	inline bool      empty     (void) const { return _v.empty ();      }
	inline size_type max_size  (void) const { return _v.size  () * 32; }

	bool operator == (const BitVector &v) const;

    protected:

	std::vector<unsigned long> _v;
	size_t              _size;

}; // template <class Vector> class ReverseVector

// Vector traits for BitVector wrapper
template <>
struct VectorTraits<BitVector>
{ 
	typedef BitVector VectorType;
	typedef VectorCategories::DenseZeroOneVectorTag<VectorTraits<VectorType> > VectorCategory; 
};

} // namespace LinBox

#include "linbox/vector/bit-vector.inl"

#endif // __BIT_VECTOR_H
