/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/blackbox/sparse.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001-2002 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2002-08-06  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Renamed to sparse.h from sparse0.h
 * ------------------------------------
 * Modified by Bradford Hovinen <hovinen@cis.udel.edu>
 * Modified by Jean-Guillaume Dumas <Jean-Guillaume.Dumas@imag.fr>
 * 	       28.08.2002 : added back : field()
 *
 * Refactoring:
 *   - Eliminated SparseMatrixAux and moved that functionality into Sparse0
 *   - Made SparseMatrixBase parameterized only on the element type
 *   - New read/write implementations for SparseMatrixBase, supporting multiple
 *     formats
 *   - Eliminated Gaussian elimination code
 *   - Added iterators, including ColOfRowsIterator, RawIterator, and
 *     RawIndexIterator
 *   - Eliminated operator []; added getEntry; changed put_value to setEntry
 * ------------------------------------
 * 
 * See COPYING for license information.
 */

#ifndef __BLACKBOX_SPARSE_H
#define __BLACKBOX_SPARSE_H

#include "linbox-config.h"
#include "linbox/matrix/sparse.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/vector/vector-traits.h"
#include "linbox/vector/stream.h"
#include "linbox/util/field-axpy.h"
#include <linbox/blackbox/blackbox-interface.h>

#ifdef __LINBOX_PARALLEL
#include <linbox/blackbox/blackbox_parallel.h>
#endif

#ifdef __LINBOX_XMLENABLED

#include "linbox/util/xml/linbox-reader.h"
#include "linbox/util/xml/linbox-writer.h"

using LinBox::Reader;
using LinBox::Writer;

#include <iostream>
#include <string>

using std::istream;
using std::ostream;

#endif


// Namespace in which all LinBox library code resides
namespace LinBox
{

	/** \brief vector of sparse rows.

	 * This is a generic black box for a sparse matrix. It inherits
	 * \ref{SparseMatrixBase}, which implements all of the underlying
	 * accessors and iterators.
\ingroup blackbox
	 */
	template <class _Field,
		  class _Row    = typename LinBox::Vector<_Field>::Sparse>
	class SparseMatrix : public BlackboxInterface, public SparseMatrixBase<typename _Field::Element, _Row>
	{
	public:

#ifdef __LINBOX_PARALLEL
		BB_list_list sub_list;
#endif

		typedef _Field Field;
		typedef typename Field::Element Element;
		typedef typename SparseMatrixBase<typename Field::Element, _Row>::Row Row;

#ifndef __LINBOX_XMLENABLED
		typedef typename SparseMatrixBase<typename Field::Element, _Row>::Format Format;
#endif

		typedef typename SparseMatrixBase<typename Field::Element, _Row>::RawIterator RawIterator;
		typedef typename SparseMatrixBase<typename Field::Element, _Row>::RawIndexedIterator RawIndexedIterator;

		/** Constructor.
		 * Builds a zero m x n matrix
		 * Note: the copy constructor and operator= will work as intended
		 *       because of STL's container design
		 * @param  F  Field over which entries exist
		 * @param  m  Row dimension
		 * @param  n  Column dimension
		 */
		SparseMatrix (const Field &F, size_t m = 0, size_t n = 0)
			: SparseMatrixBase<Element, _Row> (m, n), _F (F), _MD (F)
		{}

		/** Constructor from a vector stream
		 * @param  F  Field over which entries exist
		 * @param  stream  Stream with which to generate row vectors
		 */
		SparseMatrix (const Field &F, VectorStream<Row> &stream)
			: SparseMatrixBase<Element, _Row> (stream.size (), stream.dim ()),
			  _F (F), _MD (F)
		{
			typename SparseMatrixBase<Element, _Row>::RowIterator i;

			for (i = rowBegin (); i != rowEnd (); ++i)
				stream >> *i;
		}

		/** Copy constructor
		 */
		SparseMatrix (const SparseMatrix<Field, Row> &B)
			: SparseMatrixBase<Element, _Row> (B), _F (B._F), _MD (B._F)
		{}

#ifdef __LINBOX_XMLENABLED

		SparseMatrix(Reader &R) : SparseMatrixBase<Element, Row>(R), _F(R.Down(1)), _MD(_F) { R.Up(1);}

#endif
	      


		/** Destructor. */
		~SparseMatrix () {
#ifdef __LINBOX_PARALLEL

			BB_list_list::iterator p;

			BB_list::iterator e_p;

			for (p = sub_list. begin(); p != sub_list. end(); ++ p)

				for (e_p = p -> second. begin(); 
				     e_p != p -> second. end(); ++ e_p) {

					Thread::terminate_thread (*e_p);

					delete (*e_p);
				}

#endif
		}

		/** Matrix-vector product
		 * y = A x.
		 * @return reference to output vector y
		 * @param  x input vector
		 */
		template <class OutVector, class InVector>
		OutVector &apply (OutVector &y, const InVector &x) const {
#ifdef __LINBOX_PARALLEL
	
			return BlackboxParallel (y, *this, x, BBBase::Apply);
#else
			return _MD.vectorMul (y, *this, x);
#endif
		}


		/** Transpose matrix-vector product
		 * y = A^T x.
		 * @return reference to output vector y
		 * @param  x input vector
		 */
		template <class OutVector, class InVector>
		OutVector &applyTranspose (OutVector& y, const InVector &x) const { 

#ifdef __LINBOX_PARALLEL

			return BlackboxParallel (y, *this, x, BBBase::ApplyTranspose);
#else

			return _MD.vectorMul (y, TransposeMatrix<SparseMatrixBase<Element, _Row> > (*this), x);
#endif
		}

		/** Retreive row dimensions of Sparsemat matrix.
		 * @return integer number of rows of SparseMatrix0Base matrix.
		 */
		size_t rowdim () const { return _m; }

		/** Retreive column dimensions of Sparsemat matrix.
		 * @return integer number of columns of SparseMatrix0Base matrix.
		 */
		size_t coldim () const { return _n; }

#ifndef __LINBOX_XMLENABLED


		/** Read the matrix from a stream in the given format
		 * @param is Input stream from which to read the matrix
		 * @param format Format of input matrix
		 * @return Reference to input stream
		 */
		std::istream &read (std::istream &is, Format format = FORMAT_DETECT)
		{ return SparseMatrixBase<Element, _Row>::read (is, _F, format); }

		/** Write the matrix to a stream in the given format
		 * @param os Output stream to which to write the matrix
		 * @param format Format of output
		 * @return Reference to output stream
		 */
		std::ostream &write (std::ostream &os, Format format = FORMAT_PRETTY) const
		{ return SparseMatrixBase<Element, _Row>::write (os, _F, format); }

#else
		ostream &write(ostream &out) const
		{
			Writer W;
			if( toTag(W)) 
				W.write(out);

			return out;
		}

		bool toTag(Writer &W) const
		{
			if( SparseMatrixBase<Element, _Row>::toTag(W) ) {
				W.insertTagChild();
				_F.toTag(W);
				W.upToParent();
				return true;
			}
			else return true;
		}
      

#endif

		// JGD 28.08.2002
		/** Access to the base field
		 */
		const Field& field () const { return _F;}


	private:

		const Field                             _F;      // Field used for all arithmetic
		MatrixDomain<Field>                     _MD;     // Matrix domain for matrix operations
	};

	/** Sparse matrix factory
	 * This class inherits \ref{BlackboxFactory} and provides a method for using a
	 * \ref{SparseMatrixBase} object with integer or rational data type as input to
	 * the high-level integer and rational solutions functions.
	 */

	template <class Field,
		  class BElement = typename Field::Element,
		  class Row      = typename LinBox::Vector<Field>::Sparse,
		  class BRow     = typename LinBox::RawVector<BElement>::Sparse>
	class SparseMatrixFactory : public BlackboxFactory<Field,SparseMatrix<Field,Row> > 
	{//otot
		const SparseMatrixBase<BElement, BRow> &_A;

	public:


		SparseMatrixFactory (const SparseMatrixBase<BElement, BRow> &A)
			: _A (A) 
		{}

	
		// FIXME: This function assumes basically that the matrix is over the integers

		SparseMatrix<Field,Row> *makeBlackbox (const Field &F);

		integer &maxNorm (integer &res)
		{
			typename SparseMatrixBase<BElement, BRow>::ConstRawIterator i;

			res = 0L;

			integer tmp;

			for (i = _A.rawBegin (); i != _A.rawEnd (); ++i) {
				tmp = abs (*i);

				if (res < tmp)
					res = tmp;
			}

			return res;
		}

		size_t rowdim ()
		{ return _A.rowdim (); }
		size_t coldim ()
		{ return _A.coldim (); }

		// A better bound for determinant of an integer sparse matrix, ZW
		integer &hadamardBound (integer& res) const {

			return hadamardBound (res, typename VectorTraits<Row>::VectorCategory());

		}

		integer &hadamardBound (integer& res,  VectorCategories::SparseParallelVectorTag) const {

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRowIterator RowIterator;

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRow::second_type::const_iterator EltIterator;

			res = 1L;

			integer tmp;

			RowIterator row_p;

			EltIterator elt_p;

			for (row_p = _A. rowBegin(); row_p != _A. rowEnd(); ++ row_p) {

				tmp = 0;

				for (elt_p = row_p -> second. begin(); elt_p != row_p -> second. end(); ++ elt_p)

					tmp += static_cast<integer>(*elt_p) * (*elt_p);

				res *=tmp;
			}

			res = sqrt (res);

			return res;
		}

		integer &hadamardBound (integer& res,  VectorCategories::SparseSequenceVectorTag) const{

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRowIterator RowIterator;

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRow::const_iterator EltIterator;

			res = 1L;
		 
			integer tmp;
		 
			RowIterator row_p;
		 
			EltIterator elt_p;
		 
			for ( row_p = _A. rowBegin(); row_p != _A. rowEnd(); ++ row_p) {
			 
				tmp = 0;
		      
				for (EltIterator elt_p = row_p -> begin(); elt_p != row_p -> end(); ++ elt_p)
					tmp += static_cast<integer>(elt_p -> second) * (elt_p -> second);
		
				res *=tmp; 
			} 
		 
			res = sqrt (res); 
			return res; 
		} 

       

		integer &hadamardBound (integer& res,  VectorCategories::SparseAssociativeVectorTag) const {

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRowIterator RowIterator;

			typedef typename SparseMatrixBase<BElement, BRow>::ConstRow::const_iterator EltIterator;

			res = 1L;
		 
			integer tmp;
		 
			RowIterator row_p;
		 
			EltIterator elt_p;
		 
			for ( row_p = _A. rowBegin(); row_p != _A. rowEnd(); ++ row_p) {
			 
				tmp = 0;
			 
				for (elt_p = row_p -> begin(); elt_p != row_p -> end(); ++ elt_p)
					tmp += static_cast<integer>(elt_p -> second) * (elt_p -> second);
			 
				res *=tmp; 
			} 
		 
			res = sqrt (res); 
			return res; 
		} 

	};

	template <class Field, class _Row>
	struct MatrixTraits< SparseMatrix<Field, _Row> >
	{ 
		typedef SparseMatrix<Field, _Row> MatrixType;
		typedef typename MatrixCategories::RowMatrixTag MatrixCategory;
	};

} // namespace LinBox

#include "linbox/blackbox/sparse.inl"

#endif // __BLACKBOX_SPARSE_H
