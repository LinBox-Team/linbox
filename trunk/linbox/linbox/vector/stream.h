/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/vector/stream.h
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2002-09-05 Bradford Hovinen <bghovine@math.uwaterloo.ca>
 *
 *  - Renamed to stream.h and moved to linbox/vector
 *  - VectorFactory is now called VectorStream, which fits its purpose
 *    somewhat better
 *  - The interface is now closer to the interface for istream
 *  - RandomDenseVectorFactory, et al. are refactored into classes
 *    parameterized on the vector type and specialized appropriately. This
 *    allows, e.g. construction of a random dense vector in sparse
 *    representation and so on.
 *  - New constructor interface for RandomSparseStream accepts proportion of
 *    nonzero entries; removed existing constructors
 *  - Reindented, since the other changes are so numerous that diffs aren't a
 *    big deal anyway
 *
 * ------------------------------------
 * 2002-05-18 Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * Refactor: Create one class StandardBasisFactory, parameterized by vector
 * type, with specializations for dense, sparse map, and sparse associative
 * vectors.
 * 
 * ------------------------------------
 * Modified by Dmitriy Morozov <linbox@foxcub.org>. May 27, 2002.
 *
 * Added parametrization of the VectorCategroy tags by VectorTraits (see 
 * vector-traits.h for more details).
 * 
 * ------------------------------------
 *  
 * See COPYING for license information.
 */

#ifndef __VECTOR_STREAM_H
#define __VECTOR_STREAM_H

#include <vector>
#include <cmath>

#include "linbox/vector/vector-traits.h"
#include "linbox/util/debug.h"
#include "linbox/randiter/nonzero.h"

namespace LinBox 
{

/** Vector factory
 * This is an abstract base class that generates a sequence of vectors
 * in a generic way. Typical uses would be in tests, where the same test
 * might be run on a sequence of random vectors or on e_1, ..., e_n.
 */
template <class Vector>
class VectorStream 
{
    public:
	virtual ~VectorStream () {}

	/** Get the next vector from the factory and store it in v
	 */

	virtual Vector &get (Vector &v) = 0;

	/** Extraction operator form
	 */
	VectorStream<Vector> &operator >> (Vector &v)
		{ get (v); return *this; }

	/** Get the number of vectors to be constructed in this stream
	 */
	virtual size_t size () const = 0;

	/** Get the number of vectors constructed so far
	 */
	virtual size_t pos () const = 0;

	/** Get the dimension of each vector
	 */
	virtual size_t dim () const = 0;

	/** Return true if and only if the vector stream still has more vectors
	 * to construct
	 */
	virtual operator bool () const = 0;

	/** Reset the vector stream to the beginning.
	 */
	virtual void reset () = 0;

	/** Alias for reset
	 */
	void rewind () { reset (); }

	/** @name Legacy interface
	 * These functions provide compatiblity with older parts of the
	 * library. Their use is deprecated.
	 */

	//@{

	Vector &next (Vector &v) { return get (v); }
	size_t j () const { return pos (); }
	size_t m () const { return size (); }
	size_t n () const { return dim (); }

	//@}
};

/** Constant vector factory
 * Returns the same vector repeatedly
 */
template <class Vector>
class ConstantVectorStream : public VectorStream<Vector>
{
    public:

	/** Constructor
	 * Construct a new factory with the given field and vector size.
	 * @param v Vector to return on next
	 * @param m Number of vectors to return (0 for unlimited)
	 */
	ConstantVectorStream (Vector &v, size_t m) : _v (v), _m (m), _j (0) {}

	/** Retrieve vector
	 * @param v Vector to use
	 */
	Vector &get (Vector &v) 
		{ if (_m == 0 || _j < _m) copy (_v.begin (), _v.end (), v.begin ()); return v; }

	/** Number of vectors to be created
	 */
	size_t size () const { return _m; }

	/** Number of vectors created so far
	 */
	size_t pos () const { return _j; }

	/** Dimension of the space
	 */
	size_t dim () const { return _v.size (); }

	/** Check whether we have reached the end
	 */
	operator bool () const { return _m == 0 || _j < _m; }

	/** Reset the factory to start at the beginning
	 */
	void reset () { _j = 0; }

    private:
	Vector &_v;
	size_t  _m;
	size_t  _j;
};

/** Random dense vector stream
 * Generates a sequence of random dense vectors over a given field
 */
template <class Field, class Vector = typename Vector<Field>::Dense, class RandIter = typename Field::RandIter, class Trait = typename VectorTraits<Vector>::VectorCategory>
class RandomDenseStream : public VectorStream<Vector>
{
    public:

	/** Constructor
	 * Construct a new stream with the given field and vector size.
	 * @param F Field over which to create random vectors
	 * @param n Size of vectors
	 * @param m Number of vectors to return (0 for unlimited)
	 */
	RandomDenseStream (const Field &F, size_t n, size_t m = 0);

	/** Constructor
	 * Construct a new stream with the given field and vector size.
	 * @param F Field over which to create random vectors
	 * @param n Size of vectors
	 * @param m Number of vectors to return (0 for unlimited)
	 */
	RandomDenseStream (const Field &F, const RandIter &r, size_t n, size_t m = 0);

	/** Get next element
	 * @param v Vector into which to generate random vector
	 * @return reference to new random vector
	 */
	Vector &get (Vector &v);

	/** Number of vectors to be created
	 */
	size_t size () const;

	/** Number of vectors created so far
	 */
	size_t pos () const;

	/** Dimension of the space
	 */
	size_t dim () const;

	/** Check whether we have reached the end
	 */
	operator bool () const;

	/** Reset the stream to start at the beginning
	 */
	void reset ();
};

// Specialization of random dense stream for dense vectors

template <class Field, class Vector, class RandIter, class VectorTrait>
class RandomDenseStream<Field, Vector, RandIter, VectorCategories::DenseVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	RandomDenseStream (const Field &F, size_t n, size_t m = 0)
		: _F (F), _r (F), _n (n), _m (m), _j (0)
	{}

	RandomDenseStream (const Field &F, const RandIter &r, size_t n, size_t m = 0)
		: _F (F), _r (r), _n (n), _m (m), _j (0)
	{}

	Vector &get (Vector &v) 
	{
		typename Vector::iterator i;

		if (_m > 0 && _j++ >= _m)
			return v;

		for (i = v.begin (); i != v.end (); i++)
			_r.random (*i);

		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }

    private:
	const Field &_F;
	RandIter     _r;
	size_t       _n;
	size_t       _m;
	size_t       _j;
};

/** Random sparse vector stream
 * Generates a sequence of random sparse vectors over a given field
 */
template <class Field, class Vector = typename Vector<Field>::Sparse, class RandIter = typename Field::RandIter, class Trait = typename VectorTraits<Vector>::VectorCategory>
class RandomSparseStream : public VectorStream<Vector>
{
    public:

	/** Constructor
	 * Construct a new stream with the given field and vector size.
	 * @param F Field over which to create random vectors
	 * @param n Size of vectors
	 * @param p Proportion of nonzero entries
	 * @param m Number of vectors to return (0 for unlimited)
	 */
	RandomSparseStream (const Field &F, size_t n, double p, size_t m = 0);

	/** Constructor
	 * Construct a new stream with the given field and vector size.
	 * @param F Field over which to create random vectors
	 * @param n Size of vectors
	 * @param p Proportion of nonzero entries
	 * @param m Number of vectors to return (0 for unlimited)
	 */
	RandomSparseStream (const Field &F, const RandIter &r, size_t n, double p, size_t m = 0);

	/** Get next element
	 * @param v Vector into which to generate random vector
	 * @return reference to new random vector
	 */
	Vector &get (Vector &v);

	/** Number of vectors to be created
	 */
	size_t size () const;

	/** Number of vectors created so far
	 */
	size_t pos () const;

	/** Dimension of the space
	 */
	size_t dim () const;

	/** Check whether we have reached the end
	 */
	operator bool () const;

	/** Reset the stream to start at the beginning
	 */
	void reset ();

	/** Set the probability of a nonzero entry
	 */
	void setP (double p);
};

// Specialization of RandomSparseStream for dense vectors

template <class Field, class Vector, class RandIter, class VectorTrait>
class RandomSparseStream<Field, Vector, RandIter, VectorCategories::DenseVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	RandomSparseStream (const Field &F, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, typename Field::RandIter (F)), _n (n), _p (p), _m (m), _j (0)
		{ linbox_check ((p >= 0.0) && (p <= 1.0)); _F.init (_zero, 0); }

	RandomSparseStream (const Field &F, const RandIter &r, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, r), _n (n), _p (p), _m (m), _j (0)
		{ linbox_check ((p >= 0.0) && (p <= 1.0)); _F.init (_zero, 0); }

	Vector &get (Vector &v) 
	{
		double val;

		if (_m > 0 && _j++ >= _m)
			return v;

		for (typename Vector::iterator i = v.begin (); i != v.end (); ++i) {
			val = (double) ((unsigned long) rand ()) / (0.5 * (double) ((unsigned long) -1));

			if (val < _p)
				_r.random (*i);
			else
				_F.assign (*i, _zero);
		}

		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }
	void setP (double p) { linbox_check ((p >= 0.0) && (p <= 1.0)); _p = p; }

    private:
	const Field                      &_F;
	typename Field::Element           _zero;
	NonzeroRandIter<Field, RandIter>  _r;
	size_t                            _n;
	double                            _p;
	size_t                            _m;
	size_t                            _j;
};

// Specialization of RandomSparseStream for sparse sequence vectors

template <class Field, class Vector, class RandIter, class VectorTrait>
class RandomSparseStream<Field, Vector, RandIter, VectorCategories::SparseSequenceVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	RandomSparseStream (const Field &F, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, typename Field::RandIter (F)), _n (n), _m (m), _j (0)
		{ setP (p); }

	RandomSparseStream (const Field &F, const RandIter &r, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, r), _n (n), _p (p), _m (m), _j (0)
		{ setP (p); }

	Vector &get (Vector &v) 
	{
		typename Field::Element x;
		size_t i = (size_t) -1;
		double val;
		int skip;

		if (_m > 0 && _j++ >= _m)
			return v;

		v.clear ();

		while (1) {
			val = (double) ((unsigned long) rand ()) / (0.5 * (double) ((unsigned long) -1));
			skip = (int) (ceil (log (val) * _1_log_1mp));

			if (skip <= 0)
				i++;
			else
				i += skip;

			if (i >= _n) break;

			_r.random (x);
			v.push_back (std::pair<size_t, typename Field::Element> (i, x));
		}

		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }

	void setP (double p)
	{
		linbox_check ((p >= 0.0) && (p <= 1.0)); 
		_p = p;
		_1_log_1mp   = 1 / log (1 - _p);
	}

    private:
	const Field                      &_F;
	NonzeroRandIter<Field, RandIter>  _r;
	size_t                            _n;
	double                            _p;
	double                            _1_log_1mp;
	size_t                            _m;
	size_t                            _j;
};

// Specialization of RandomSparseStream for sparse associative vectors

template <class Field, class Vector, class RandIter, class VectorTrait>
class RandomSparseStream<Field, Vector, RandIter, VectorCategories::SparseAssociativeVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	RandomSparseStream (const Field &F, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, typename Field::RandIter (F)), _n (n), _k ((long) (p * n)), _j (0), _m (m)
	{}

	RandomSparseStream (const Field &F, const RandIter &r, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, r), _n (n), _k ((long) (p * n)), _j (0), _m (m)
	{}

	Vector &get (Vector &v) 
	{
		typename Field::Element x;
		int i, idx;

		if (_m > 0 && _j++ >= _m)
			return v;

		v.clear ();

		for (i = 0; i < _k; i++) {
			_r.random (x);
			while (!_F.isZero (v[(idx = rand () % _n)]));
			v[idx] = x;
		}

		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }
	void setP (double p) { _k = (long) (p * n); }

    private:
	const Field                      &_F;
	NonzeroRandIter<Field, RandIter>  _r;
	size_t                            _n;
	long                              _k;
	size_t                            _j;
	size_t                            _m;
};

// Specialization of RandomSparseStream for sparse parallel vectors

template <class Field, class Vector, class RandIter, class VectorTrait>
class RandomSparseStream<Field, Vector, RandIter, VectorCategories::SparseParallelVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	RandomSparseStream (const Field &F, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, typename Field::RandIter (F)), _n (n), _m (m), _j (0)
		{ setP (p); }

	RandomSparseStream (const Field &F, const RandIter &r, size_t n, double p, size_t m = 0)
		: _F (F), _r (F, r), _n (n), _m (m), _j (0)
		{ setP (p); }

	Vector &get (Vector &v) 
	{
		typename Field::Element x;
		size_t i = (size_t) -1;
		double val;
		int skip;

		if (_m > 0 && _j++ >= _m)
			return v;

		v.first.clear ();
		v.second.clear ();

		while (1) {
			val = (double) ((unsigned long) rand ()) / (0.5 * (double) ((unsigned long) -1));
			skip = (int) (ceil (log (val) * _1_log_1mp));

			if (skip <= 0)
				i++;
			else
				i += skip;

			if (i >= _n) break;

			_r.random (x);
			v.first.push_back (i);
			v.second.push_back (x);
		}

		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }

	void setP (double p)
	{
		linbox_check ((p >= 0.0) && (p <= 1.0)); 
		_p = p;
		_1_log_1mp   = 1 / log (1 - _p);
	}

    private:
	const Field                      &_F;
	NonzeroRandIter<Field, RandIter>  _r;
	size_t                            _n;
	double                            _p;
	double                            _1_log_1mp;
	size_t                            _m;
	size_t                            _j;
};

/** Stream for e_1,...,e_n
 * Generates the sequence e_1,...,e_n over a given field
 * 
 * This class is generic with respect to the underlying vector
 * representation.
 */

template <class Field, class Vector, class Trait = typename VectorTraits<Vector>::VectorCategory>
class StandardBasisStream : public VectorStream<Vector>
{
    public:

	/** Constructor
	 * Construct a new stream with the given field and vector size.
	 * @param F Field over which to create vectors
	 * @param n Size of vectors
	 */
	StandardBasisStream (Field &F, size_t n);

	/** Get next element
	 * @param v Vector into which to generate vector
	 * @return reference to new vector
	 */
	Vector &get (Vector &v);

	/** Number of vectors to be created
	 */
	size_t size () const;

	/** Number of vectors created so far
	 */
	size_t pos () const;

	/** Dimension of the space
	 */
	size_t dim () const;

	/** Check whether we have reached the end
	 */
	operator bool () const;

	/** Reset the stream to start at the beginning
	 */
	void reset ();

    private:
	const Field              &_F;
	size_t                    _n;
	size_t                    _j;
};

// Specialization of standard basis stream for dense vectors

template <class Field, class Vector, class VectorTrait>
class StandardBasisStream<Field, Vector, VectorCategories::DenseVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	StandardBasisStream (const Field &F, size_t n)
		: _F (F), _n (n), _j (0)
	{}

	Vector &get (Vector &v) 
	{
		static typename Field::Element zero;
		typename Vector::iterator i;
		size_t idx;

		for (i = v.begin (), idx = 0; i != v.end (); i++, idx++) {
			if (idx == _j)
				_F.init (*i, 1);
			else
				_F.assign (*i, zero);
		}

		_j++;

		return v;
	}

	size_t size () const { return _n; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _j < _n; }
	void reset () { _j = 0; }

    private:
	const Field              &_F;
	size_t                    _n;
	size_t                    _j;
};

// Specialization of standard basis stream for sparse sequence vectors

template <class Field, class Vector, class VectorTrait>
class StandardBasisStream<Field, Vector, VectorCategories::SparseSequenceVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	StandardBasisStream (Field &F, size_t n)
		: _F (F), _n (n), _j (0)
	{ _F.init (_one, 1); }

	Vector &get (Vector &v) 
	{
		v.clear ();

		if (_j < _n)
			v.push_back (std::pair <size_t, typename Field::Element> (_j++, _one));

		return v;
	}

	size_t size () const { return _n; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _j < _n; }
	void reset () { _j = 0; }

    private:
	const Field              &_F;
	size_t                    _n;
	size_t                    _j;
	typename Field::Element   _one;
};

// Specialization of standard basis stream for sparse associative vectors

template <class Field, class Vector, class VectorTrait>
class StandardBasisStream<Field, Vector, VectorCategories::SparseAssociativeVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	StandardBasisStream (Field &F, size_t n)
		: _F (F), _n (n), _j (0)
	{ _F.init (_one, 1); }

	Vector &get (Vector &v) 
	{
		v.clear ();

		if (_j < _n)
			v.insert (std::pair <size_t, typename Field::Element> (_j++, _one));

		return v;
	}

	size_t pos () const { return _j; }
	size_t size () const { return _n; }
	size_t dim () const { return _n; }
	operator bool () const { return _j < _n; }
	void reset () { _j = 0; }

    private:
	const Field              &_F;
	size_t                    _n;
	size_t                    _j;
	typename Field::Element   _one;
};

// Specialization of standard basis stream for sparse parallel vectors

template <class Field, class Vector, class VectorTrait>
class StandardBasisStream<Field, Vector, VectorCategories::SparseParallelVectorTag<VectorTrait> > : public VectorStream<Vector>
{
    public:

	StandardBasisStream (Field &F, size_t n)
		: _F (F), _n (n), _j (0)
	{ _F.init (_one, 1); }

	Vector &get (Vector &v) 
	{
		v.first.clear ();
		v.second.clear ();

		if (_j < _n) {
			v.first.push_back (_j++);
			v.second.push_back (_one);
		}

		return v;
	}

	size_t size () const { return _n; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _j < _n; }
	void reset () { _j = 0; }

    private:
	const Field              &_F;
	size_t                    _n;
	size_t                    _j;
	typename Field::Element   _one;
};

} // namespace LinBox

#endif // __VECTOR_STREAM_H
