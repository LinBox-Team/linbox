/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/blackbox/polynomial.h
 * Copyright (C) 2005 Cl'ement Pernet
 *
 * Written by Cl'ement Pernet <Clement.Pernet@imag.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __POLYNOMIAL_H
#define __POLYNOMIAL_H

#include <linbox/blackbox/blackbox-interface.h>
#include <linbox/vector/vector-domain.h>
// Namespace in which all LinBox library code resides
namespace LinBox
{

	/** \brief represent the matrix P(A) where A is a blackbox and P a polynomial
	    
	\ingroup blackbox
	
	*/
	template <class Blackbox, class Polynomial>
	class PolynomialBB : public BlackboxInterface
	{
	public:
		
		typedef typename Blackbox::Field Field;
		typedef typename Blackbox::Element Element;
		
		/** Constructor from a black box and a polynomial.
		 */
		PolynomialBB (const Blackbox& A, const Polynomial& P) : _A_ptr(&A), _P_ptr(&P), _VD(A.field()) {}
		
		PolynomialBB (const Blackbox *A_ptr, const Polynomial * P_ptr): _A_ptr(A_ptr), _P_ptr(P_ptr), _VD(A_ptr->field())
		{
		}
		
		/** Copy constructor.
		 * Creates new black box objects in dynamic memory.
		 * @param M constant reference to compose black box matrix
		 */
		PolynomialBB (const PolynomialBB<Blackbox, Polynomial> &M) : _A_ptr(M._A_ptr), _P_ptr(M._P_ptr), _VD(M._VD)
		{
		}

		/// Destructor
		~PolynomialBB (void)
		{
		}


		/** Application of BlackBox matrix.
		 * y = P(A)x
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		template <class Vector1, class Vector2>
		inline Vector1 &apply (Vector1 &y, const Vector2 &x) const
		{
			Vector2 u (x);
			Vector2 v(u.size());
			_VD.mul( y, x, _P_ptr->operator[](0) );
			for (int i=1; i<_P_ptr->size(); ++i){
				_A_ptr->apply( v, u );
				_VD.axpyin( y, _P_ptr->operator[](i), v);
				u=v;
			}
			return y;
		}


		/** Application of BlackBox matrix transpose.
		 * y= transpose(A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		template <class Vector1, class Vector2>
		inline Vector1 &applyTranspose (Vector1 &y, const Vector2 &x) const
		{
			Vector2 u( x );
			Vector2 v(u.size());
			_VD.mul( y, x, _P_ptr->operator[](0));
			for (int i=1; i<_P_ptr->size(); ++i){
				_A_ptr->applyTranspose( v, u );
				_VD.axpyin( y, _P_ptr->operator[](i), v);
				u=v;
			}
			return y;
		}

            
		template<typename _Tp1, class Poly1 = typename Polynomial::template rebind<_Tp1>::other> 
		struct rebind 
		{ typedef PolynomialBB<typename Blackbox::template rebind<_Tp1>::other, Poly1> other; };



		/** Retreive row dimensions of BlackBox matrix.
		 * This may be needed for applying preconditioners.
		 * Required by abstract base class.
		 * @return integer number of rows of black box matrix.
		 */
		size_t rowdim (void) const
		{
			if (_A_ptr != 0) 
				return _A_ptr->rowdim ();
			else 
				return 0;
		}
    
		/** Retreive column dimensions of BlackBox matrix.
		 * Required by abstract base class.
		 * @return integer number of columns of black box matrix.
		 */
		size_t coldim (void) const 
		{
			if (_A_ptr != 0) 
				return _A_ptr->coldim ();
			else 
				return 0;
		}
	       

		const Field& field() const {return _A_ptr->field();}
	    private:

		// Pointers to A and P
		const Blackbox *_A_ptr;
		const Polynomial *_P_ptr;
		const VectorDomain<Field> _VD;

	};

} // namespace LinBox

#endif // __POLYNOMIAL_H
