/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* Copyright (C) 2007 LinBox
 * Written by <Jean-Guillaume.Dumas@imag.fr>
 *
 *
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

/*! @file randiter/random-prime.h
 * @ingroup randiter
 * @brief Generates random positive prime \ref integers.
 */

#ifndef __LINBOX_random_prime_iterator_H
#define __LINBOX_random_prime_iterator_H
#include <linbox/integer.h>
#include <linbox/util/timer.h>

namespace LinBox
{

	/*! @ingroup primes
	 * @brief Random Prime Generator.
	 * Generates prime of specified length.
	 */
	class RandomPrimeIterator {

		int 	_bits;  //!< common lenght of all primes
		integer _shift; //!< @internal used to
		integer _prime; //!< the generated prime.

	public:
		/*! Constructor.
		 * @param bits size of primes (in bits)
		 * @param seed if \c 0 a seed will be generated, otherwise, the
		 * provided seed will be use.
		 */
		RandomPrimeIterator(int bits = 30, unsigned long seed = 0) :
			_bits(bits)
		{
			_shift = integer(1)<<_bits;
			if (! seed)
				RandomPrimeIterator::setSeed( BaseTimer::seed() );
			else
				RandomPrimeIterator::setSeed( seed );

			integer::random(_prime,_bits-1);
			_prime = _shift - _prime;
			nextprime( _prime, _prime);
		}

		// define the prime type
		typedef integer Prime_Type;

		/** @brief operator++()
		 *  creates a new random prime.
		 */
		inline RandomPrimeIterator &operator ++ ()
		{
			integer::random(_prime,_bits-1);
			_prime = _shift - _prime;
			nextprime( _prime, _prime);
			return *this;
		}

		/** @brief get the random prime.
		 *  returns the actual prime.
		 */
		Prime_Type &operator *  ()
		{
			return _prime;
		}
		Prime_Type & randomPrime()
		{
			return _prime;
		}

		/** @brief Sets the seed.
		 *  Set the random seed to be \p ul.
		 *  @param ul the new seed.
		 */
		void static setSeed(unsigned long ul)
		{
			integer::seeding(ul);
		}


	};
}

#endif //__LINBOX_random_prime_iterator_H

