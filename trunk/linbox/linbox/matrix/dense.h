/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/matrix/dense.h
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
 * 2003-01-11  Bradford Hovinen  <bghovinen@math.uwaterloo.ca>
 *
 * Move from blackbox/dense-base.h to matrix/dense.h
 * --------------------------------------------------------
 * 2002-11-29  Bradford Hovinen  <bghovinen@math.uwaterloo.ca>
 *
 * Swap the order of arguments in read and write, so that it is consistent with
 * SparseMatrixBase
 * --------------------------------------------------------
 * 2002-10-28  Bradford Hovinen  <bghovinen@math.uwaterloo.ca>
 *
 * Rename ColOfRowsIterator as RowIterator; similarly with RowOfColsIterator
 * --------------------------------------------------------
 * 2002-10-27  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Split out container/iterator functionality into DenseMatrixBase
 * --------------------------------------------------------
 *
 * See COPYING for license information
 */

#ifndef __MATRIX_DENSE_H
#define __MATRIX_DENSE_H

#include <iostream>
#include <vector>
#include <fstream>

#include "linbox/vector/subiterator.h"
#include "linbox/vector/subvector.h"
#include "linbox/vector/stream.h"
#include "linbox/matrix/matrix-domain.h"

namespace LinBox
{

/** Blackbox dense matrix template. This is a class of dense matrices
 * templatized by the {@link Fields field} in which the elements
 * reside. The matrix is stored as a one dimensional STL vector of
 * the elements, by rows. The interface provides for iteration
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
  
template <class _Element>
class DenseMatrixBase
{
    public:

	typedef _Element Element;
	typedef typename RawVector<Element>::Dense Rep;

	/** Constructor.
	 */
	DenseMatrixBase ()
		: _rows (0), _cols (0)
	{}

	/** Constructor.
	 * @param  m  row dimension
	 * @param  n  column dimension
	 */
	DenseMatrixBase (size_t m, size_t n)
		: _rep (m * n), _rows (m), _cols (n), _ptr(&_rep[0])
	{}

	/** Copy constructor
	 */
	DenseMatrixBase (const DenseMatrixBase &M)
		: _rep (M._rep),_rows (M._rows), _cols (M._cols), _ptr(&_rep[0])
	{}

	/** Operator =
	 */
	DenseMatrixBase& operator= (const DenseMatrixBase& M) {
		(*this)._rep  = M._rep;
		(*this)._rows = M._rows;
		(*this)._cols = M._cols;
		(*this)._ptr  = &_rep[0];
		return (*this);
	}

	/** Get a pointer on the storage of the elements
	 * @return a pointer on Elements
	 */
	Element* FullIterator()const {return const_cast<Element*>(&_rep[0]);}

	/** Get the number of rows in the matrix
	 * @return Number of rows in matrix
	 */
	size_t rowdim () const
		{ return _rows; }

	/** Get the number of columns in the matrix
	 * @return Number of columns in matrix
	 */
	size_t coldim () const
		{ return _cols; }

	/** Resize the matrix to the given dimensions
	 * The state of the matrix's entries after a call to this method is
	 * undefined
	 * @param m Number of rows
	 * @param n Number of columns
	 */
	void resize (size_t m, size_t n)
	{
		_rows = m;
		_cols = n;
		_rep.resize (m * n);
	}

	/** @name Input and output
	 */

	//@{

	/** Read the matrix from an input stream
	 * @param file Input stream from which to read
	 * @param F Field over which to read
	 */
	template <class Field>
	std::istream &read (std::istream &file, const Field &F);

	/** Write the matrix to an output stream
	 * @param os Output stream to which to write
	 * @param F Field over which to write
	 */
	template <class Field>
	std::ostream &write (std::ostream &os, const Field &F) const;

	//@}

	/** @name Access to matrix elements
	 */

	//@{

	/** Set the entry at the (i, j) position to a_ij.
	 * @param i Row number, 0...rowdim () - 1
	 * @param j Column number 0...coldim () - 1
	 * @param a_ij Element to set
	 */
	void setEntry (size_t i, size_t j, const Element &a_ij)
		{ _rep[i * _cols + j] = a_ij; }

	/** Get a writeable reference to the entry in the (i, j) position.
	 * @param i Row index of entry
	 * @param j Column index of entry
	 * @return Reference to matrix entry
	 */
	Element &refEntry (size_t i, size_t j)
		{ return _rep[i * _cols + j]; }

	/** Get a read-only reference to the entry in the (i, j) position.
	 * @param i Row index
	 * @param j Column index
	 * @return Const reference to matrix entry
	 */
	const Element &getEntry (size_t i, size_t j) const
		{ return _rep[i * _cols + j]; }

	/** Copy the (i, j) entry into x, and return a reference to x.
	 * This form is more in the Linbox style and is provided for interface
	 * compatibility with other parts of the library
	 * @param x Element in which to store result
	 * @param i Row index
	 * @param j Column index
	 * @return Reference to x
	 */
	Element &getEntry (Element &x, size_t i, size_t j) const
		{ x = _rep[i * _cols + j]; return x; }

	/** @name Column of rows iterator
	 * The column of rows iterator traverses the rows of the
	 * matrix in ascending order. Dereferencing the iterator yields
	 * a row vector in dense format
	 */

	typedef Subvector<typename Rep::iterator> Row;  
	typedef Subvector<typename Rep::const_iterator> ConstRow;  

	class RowIterator;    
	class ConstRowIterator;

	RowIterator rowBegin ();  
	RowIterator rowEnd ();
	ConstRowIterator rowBegin () const;        
	ConstRowIterator rowEnd () const;

	/** @name Row of columns iterator
	 * The row of columns iterator traverses the columns of the
	 * matrix in ascending order. Dereferencing the iterator yields
	 * a column vector in dense format
	 */

	typedef Subvector<Subiterator<typename Rep::iterator> > Col;
	typedef Subvector<Subiterator<typename Rep::const_iterator> > ConstCol;
	typedef Col Column;
	typedef ConstCol ConstColumn;

	class ColIterator;
	class ConstColIterator;
    
	ColIterator colBegin ();
	ColIterator colEnd ();
	ConstColIterator colBegin () const;    
	ConstColIterator colEnd () const;

	/** @name Raw iterator
	 *
	 * The raw iterator is a method for accessing all entries in the matrix
	 * in some unspecified order. This can be used, e.g. to reduce all
	 * matrix entries modulo a prime before passing the matrix into an
	 * algorithm.
	 */

	typedef typename Rep::iterator RawIterator;
	typedef typename Rep::const_iterator ConstRawIterator;
    
	RawIterator rawBegin ();		  
	RawIterator rawEnd ();
	ConstRawIterator rawBegin () const;
	ConstRawIterator rawEnd () const;

	/** @name Raw Indexed iterator
	 * Like the raw iterator, the indexed iterator is a method for 
	 * accessing all entries in the matrix in some unspecified order. 
	 * At each position of the the indexed iterator, it also provides 
	 * the row and column indices of the currently referenced entry.
	 * This is provided through it's rowIndex() and colIndex() functions.
	 */

        class RawIndexedIterator;
        typedef const RawIndexedIterator ConstRawIndexedIterator;

        RawIndexedIterator rawIndexedBegin();
        RawIndexedIterator rawIndexedEnd();   
	ConstRawIndexedIterator rawIndexedBegin() const;
        ConstRawIndexedIterator rawIndexedEnd() const;   
    
	/** Retrieve a reference to a row.
	 * Since rows may also be indexed, this allows A[i][j] notation
	 * to be used.
	 * @param i Row index
	 */
	Row operator[] (size_t i)
		{ return Row (_rep.begin () + i * _cols, _rep.begin () + i * _cols + _cols); }

	ConstRow operator[] (size_t i) const
		{ return Row (_rep.begin () + i * _cols, _rep.begin () + i * _cols + _cols); }

	//@}

	/** Compute column density
	 */

	template <class Vector>
	Vector &columnDensity (Vector &v) const
		{ std::fill (v.begin (), v.end (), _rows); }

    protected:

	std::vector<Element>  _rep;
	size_t                _rows, _cols;
	Element *             _ptr;
};

template <class Element>
struct MatrixTraits< DenseMatrixBase<Element> >
{ 
	typedef DenseMatrixBase<Element> MatrixType;
	typedef typename MatrixCategories::RowColMatrixTag<MatrixTraits<MatrixType> > MatrixCategory; 
};

} // namespace LinBox

#include "dense.inl"

#endif // __MATRIX_DENSE_H
