/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/blackbox/dense.h
 * Copyright (C) 2001 B. David Saunders,
 *               2001-2002 Bradford Hovinen,
 *               2002 Zhendong Wan
 *
 * Written by B. David Saunders <saunders@cis.udel.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>,
 *            Zhendong Wan <wan@mail.eecis.udel.edu>
 *
 * evolved from dense-matrix.h by -bds, Zhendong Wan
 *
 * --------------------------------------------------------
 * 2002-10-27  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Split out container/iterator functionality into DenseMatrixBase
 * --------------------------------------------------------
 * 2002-08-09  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Renamed file from dense-matrix1.h to dense.h
 * --------------------------------------------------------
 *
 * See COPYING for license information
 */

#ifndef __DENSE_H
#define __DENSE_H

#include <iostream>
#include <vector>
#include <fstream>

#include "linbox/blackbox/archetype.h"
#include "linbox/vector/subiterator.h"
#include "linbox/vector/subvector.h"
#include "linbox/vector/stream.h"
#include "linbox/field/vector-domain.h"
#include "linbox/blackbox/dense-base.h"

namespace LinBox
{

/** Blackbox dense matrix template. This is a class of dense matrices
 * templatized by the {@link Fields field} in which the elements
 * reside. The matrix is stored as a one dimensional STL vector of
 * the elements, in row major order. The interface provides for iteration
 * over rows and over columns.
 *
 * The class also conforms to the {@link Archetypes archetype} for
 * \Ref{Blackbox Matrices}.
 *
 * Currently, only dense vectors are supported when doing matrix-vector
 * applies.
 *
 * @param Field \Ref{LinBox} field
 */

template <class Field, class Vector = typename LinBox::Vector<Field>::Dense>
class DenseMatrix : public DenseMatrixBase<typename Field::Element>, public BlackboxArchetype<Vector>
{
    public:
	typedef typename Field::Element   Element;
	typedef typename Vector::iterator pointer;

	/** Constructor of a m by n matrix with initial entries which are the 
	 * default constructor value of the field's element type.
	 * @param  F the field of entries; passed so that arithmetic may be done on elements. 
	 * @param  m  row dimension
	 * @param  n  column dimension
	 */
	DenseMatrix (const Field &F, size_t m, size_t n)
		: DenseMatrixBase<Element> (m, n), _F (F), _VD (F)
	{}

	/** Constructor of a m by n matrix with entries created by a random iterator.
	 * @param  F the field of entries; passed so that arithmetic may be done on elements. 
	 * @param  m  row dimension
	 * @param  n  column dimension
	 * @para iter, random iterator
	 */
	template<class RandIter>
	DenseMatrix (const Field &F, size_t m, size_t n, RandIter &iter)
		: DenseMatrixBase<Element> (m, n), _F (F), _VD (F)
	{
		for (typename Vector::iterator p = _rep.begin (); p != _rep.end (); ++p)
			iter.random (*p);
	}
    
	/** Constructor using a finite vector stream (stream of the rows).
	 * @param  F The field of entries; passed so that arithmetic may be done
	 *           on elements. 
	 * @param  stream A vector stream to use as a source of vectors for this
	 *                matrix
	 */
	template <class StreamVector>
	DenseMatrix (const Field &F, VectorStream<StreamVector> &stream)
		: DenseMatrixBase<Element> (stream.size (), stream.dim ()), _F (F), _VD (F)
	{
		StreamVector tmp;
		typename DenseMatrixBase<Element>::ColOfRowsIterator p;

		VectorWrapper::ensureDim (tmp, stream.dim ());

		for (p = colOfRowsBegin (); p != colOfRowsEnd (); ++p) {
			stream >> tmp;
			_VD.copy (*p, tmp);
		}
	}

	/** Constructor from a @ref{DenseMatrixBase}
	 * @param F Field over which this matrix will be
	 * @param M @ref{DenseMatrixBase} from which to get elements
	 */
	DenseMatrix (const Field &F, DenseMatrixBase<Element> &M)
		: DenseMatrixBase<Element> (M), _F (F), _VD (F)
	{}

	/** Copy constructor
	 */
	DenseMatrix (const DenseMatrix &M)
		: DenseMatrixBase<Element> (M), _F (M._F), _VD (M._F)
	{}

	/** Construct a copy of the matrix and return a pointer to it
	 * @return Pointer to copy of the matrix
	 */
	BlackboxArchetype<Vector> *clone () const 
		{ return new DenseMatrix<Field, Vector> (*this); }

	/** Get the number of rows in the matrix
	 * @return Number of rows in matrix
	 */
	size_t rowdim () const
		{ return DenseMatrixBase<Element>::rowdim (); }

	/** Get the number of columns in the matrix
	 * @return Number of columns in matrix
	 */
	size_t coldim () const
		{ return DenseMatrixBase<Element>::coldim (); }

	/** Retrieve the field over which this matrix is defined
	 * @return Reference to the underlying field
	 */
	const Field &field () const
		{ return _F;}

	/** @name Input and output
	 */

	//@{

	/** Read the matrix from an input stream
	 * @param file Input stream from which to read
	 */
	void read (std::istream &file)
		{ return read (F, file); }
    
	/** Write the matrix to an output stream
	 * @param os Output stream to which to write
	 */
	std::ostream &write (std::ostream &os = std::cout) const
		{ return write (F, file); }
 
	//@}

	/** @name Black box interface
	 */

	//@{

	/** Generic matrix-vector apply
	 * y = A * x.
	 * This version of apply allows use of arbitrary input and output vector
	 * types.
	 * @param y Output vector
	 * @param x Input vector
	 * @return Reference to output vector
	 */
	template<class Vect1, class Vect2>
	Vect1 &apply (Vect1 &y, const Vect2 &x) const;

	/** Generic in-place apply
	 * y = A * y.
	 * This version of in-place apply allows use of an arbitrary vector
	 * type. Because it performs allocation and copying, it is not
	 * recommended for general use.
	 * @param y Input vector
	 * @return Reference to output vector
	 */
	template<class Vect1>
	Vect1 &applyIn (Vect1 &y) const
	{
		std::vector<Element> x (y.begin (),y.end ());
		apply (y,x);
		return y;
	}

	/** Matrix-vector apply
	 * y = A * x
	 * This implements the @ref{BlackboxArchetype} apply requirement
	 * @param y Output vector
	 * @param x Input vector
	 * @return Reference to output vector
	 */
	Vector &apply (Vector &y, const Vector &x) const
		{ return apply<Vector, Vector> (y, x); }

	/** Iterator form of apply
	 * This form of apply takes iterators specifying the beginning and end
	 * of the vector to which to apply the matrix, and the beginning of the
	 * vector at which to store the result of application. It is generic
	 * with respect to iterator type, allowing different iterators to be
	 * used for the input and output vectors.
	 * @param out Beginning of output vector
	 * @param inbegin Beginning of input vector
	 * @param outbegin End of input vector
	 * @return Reference to beginning of output vector
	 */
	template<class Iterator1, class Iterator2 >
	Iterator1 &apply (Iterator1 out, 
			  const Iterator2 &inbegin, 
			  const Iterator2 &inend) const;

	/** Generic matrix-vector transpose apply
	 * y = A^T * x
	 * This version of applyTranspose allows use of arbitrary input and
	 * output vector types
	 * @param y Output vector
	 * @param x Input vector
	 * @return Reference to output vector
	 */
	template<class Vect1, class Vect2>
	Vect1 &applyTranspose (Vect1 &y, const Vect2 &x) const;
    
	/** Generic in-place transpose apply
	 * y = A^T * y
	 * This version of in-place transpose apply allows use of an arbitrary
	 * vector type. Because it performs allocation and copying, it is not
	 * recommended for general use.
	 * @param y Input vector
	 * @return Reference to output vector
	 */
	template<class Vect>
	Vect &applyTransposeIn (Vect &y) const
	{
		std::vector<Element> x (y.begin (), y.end ());
		applyTranspose (y, x);
		return y;
	}
  
	/** Matrix-vector transpose apply
	 * y = A^T * x
	 * This implements the @ref{BlackboxArchetype} applyTranspose
	 * requirement
	 * @param y Output vector
	 * @param x Input vector
	 * @return Reference to output vector
	 */
	Vector &applyTranspose (Vector &y, const Vector &x) const
		{ return applyTranspose<Vector,Vector> (y, x); }
    
	/** Iterator form of transpose apply
	 *
	 * This form of transpose apply takes iterators specifying the beginning
	 * and end of the vector to which to apply the matrix, and the beginning
	 * of the vector at which to store the result of application. It is
	 * generic with respect to iterator type, allowing different iterators
	 * to be used for the input and output vectors.
	 *
	 * @param out Beginning of output vector
	 * @param inbegin Beginning of input vector
	 * @param outbegin End of input vector
	 * @return Reference to beginning of output vector
	 */
	template<class Iterator1, class Iterator2>
	Iterator1 &applyTranspose (Iterator1 out, 
				   const Iterator2 &inbegin, 
				   const Iterator2 &inend) const;

	//@}

    protected:

	const Field          &_F;
	VectorDomain<Field>   _VD;
};

}

#include "dense.inl"

#endif
