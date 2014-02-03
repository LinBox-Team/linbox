/* linbox/blackbox/jit-matrix.h
 * Copyright (c) LinBox
 *
 * bds, jpm
 *
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
 * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 *.
 */

#ifndef __LINBOX_jitmatrix_H
#define __LINBOX_jitmatrix_H

#include "linbox/blackbox/blackbox-interface.h"

namespace LinBox
{

	/**

	  \brief   Example of a blackbox that is space efficient, though not time efficient.

	  \ingroup blackbox
	  Just In Time Matrix.

	  The matrix itself is not stored in memory.  Rather, an EntryGenerator
	  function is called to provide the entries.  The entry generator is
	  called once for each entry during an apply or applyTranspose
	  operation.

	  An toy example of its use is the Hilbert matrix, whose \f$i,j\f$ entry is
	  generated by the formula \f$1/(i+j+2)\f$ in zero based indexing.
	  The motivating examples were matrices also defined by formula, the Paley type matrices.
	  \sa{MSW07}% ISSAC 07 paper
	  In that context block structured turned out to be essential and the
	  JIT_Matrix class is primarily intended for block structured matrices,
	  the JIT entries being matrix blocks.

	  @param _Field only need provide the \c init() and \c axpyin() functions.

	  @param JIT_EntryGenerator \c gen() is a function object defining the
	  matrix by  providing <code>gen(e, i, j)</code> which sets field element e to the \c i,j entry
	  of the matrix. Indexing is zero based.

*/


	template <class _Field, class JIT_EntryGenerator>
	class JIT_Matrix {
	public:

		typedef _Field Field;
		typedef typename Field::Element Element;
		typedef MatrixCategories::BlackboxTag MatrixCategory;

		/**
		 * m by n matrix is constructed.
		 * JIT(Field::Element& e, size_t i, size_t j) is a function object which
		 * assigns the i,j entry to e (and returns a reference to e)
		 * and must be valid for 0 <= i < m, 0 <= j < n.
		 **/

		JIT_Matrix (const _Field& F, const size_t m, const size_t n,
			    const JIT_EntryGenerator& JIT) :
			_field(&F), _m(m), _n(n), _gen(JIT)
		{};

		template<class OutVector, class InVector>
		OutVector& apply (OutVector& y, const InVector& x) const;

		template<class OutVector, class InVector>
		OutVector& applyTranspose (OutVector& y, const InVector& x) const;
		size_t rowdim (void) const { return _m; }
		size_t coldim (void) const { return _n; }
		const Field& field() const { return *_field; }

	protected:

		// Field for arithmetic
		const Field *_field;

		// Number of rows and columns of matrix.
		size_t _m;
		size_t _n;

		// STL vector of field elements used in applying matrix.
		JIT_EntryGenerator _gen;

	}; // class JIT_Matrix


	// Method implementations

	template <class Field, class JIT_EntryGenerator>
	template <class OutVector, class InVector>
	inline OutVector& JIT_Matrix<Field, JIT_EntryGenerator>::apply (OutVector& y, const InVector& x) const
	{
		Element entry;
		field().assign(entry,field().zero);
		for (size_t i = 0; i < _m; ++i) {
			field().assign(y[i], field().zero);
			for (size_t j = 0; j < _n; ++j) {
				_gen(entry, i, j);

				field().axpyin (y[i], entry, x[j]);
			}
		}
		return y;
	} //apply


	template <class Field, class JIT_EntryGenerator>
	template <class OutVector, class InVector>
	inline OutVector& JIT_Matrix<Field, JIT_EntryGenerator>::applyTranspose (OutVector& y, const InVector& x) const
	{
		Element entry;
		field().assign(entry,field().zero);
		for (size_t i = 0; i < _m; ++i) {
			field().assign(y[i], field().zero);
			for (size_t j = 0; j < _n; ++j) {
				field().axpyin ( y[i], x[j], _gen(entry, j, i) );
			}
		}
		return y;
	} // applyTranspose



	// Example: Generator to create psuedo-random entries
	// !WARNING! repeated calls will give different values for the same entry

	template < class Field >
	class JIT_RandomEntryGenerator {
		typename Field::RandIter _r;
		size_t _b;

	public:
		JIT_RandomEntryGenerator(Field& F, size_t b) :
			_r(F), _b(b)
		{}

		typename Field::Element& operator()(typename Field::Element& e,
						    size_t k,  size_t l)
		{
			return _r.random(e);
		}
	};


} // namespace LinBox

#endif // __LINBOX_jitmatrix_H


// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,:0,t0,+0,=s
// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:

