/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/** \file archetype.h linbox/blackbox/archetype.h
\brief apply, applytranspose, rowdim, coldim
*/

/* linbox/blackbox/archetype.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *            and bds.
 *
 * See COPYING for license information.
 */

#ifndef __BLACKBOX_ARCHETYPE_H
#define __BLACKBOX_ARCHETYPE_H

#ifdef __LINBOX_XMLENABLED

#include "linbox/util/xml/linbox-writer.h"
#include <iostream>

using std::ostream;

#endif

namespace LinBox
{

	/*-  Note the original archetype concept has been given 
         * up in favor of supporting template members.


	 * Found in file \URL{linbox/blackbox/archetype.h}.
	 * Base class from which derived concrete blackbox classes.
	 * Unlike the LinBox field common object interface,
	 * the common object interface for LinBox BlackBoxes does not require
	 * the copy constructor.  All object management routines are given 
	 * through virtual clone and killclone methods.  This allows the base
	 * object to be the archetype, which is not possible for LinBox fields. 
	 *
	 * In general, there are three uses of archetypes:
	 * \begin{enumerate}
	 * \item To define the interface, i.e., document what an
	 *       explicitly designed field class must have.  This is
	 *       useful, for instance, in building a wrapper or adaptor
	 *       to an existing library.
	 * \item To distribute compiled code and ease the testing of
	 *       library components.
	 * \item To control code bloat.
	 * \end{enumerate}
	 * Because of their use of virtual member funtions, these archetypes can be 
	 * inefficient.
	 *
	 * @param Vector \Ref{LinBox} dense or sparse vector of field elements
	 */

	/*- 
	@memo BlackBox base class and archetype 
	@doc 
	This archetype is an abstract base class for blackbox matrix classes.
	The key member functions are {\tt apply, applyTranspose, rodwim, coldim}.
	They are pure virtual, and hence are implemented in each child class.  
	
	Concrete classes inheriting from the archetype
	use a variety of representation schemes for matrices internally. 
	All provide the blackbox interface described here and can be used 
	interchangably in blackbox algorithms.
	Some also implement a dense matrix or sparse matrix interface to support elimination
	techniques.  Each has unique constructor(s) reflecting it's specific scheme for representing
	a linear operator.

	Algorithms written with a Blackbox template parameter 
	may be compiled against any of these classes specifically or may be separately compiled
	against the archetype.  Algorithms may also be written with a BlackboxArchetype parameter
	and then called with an instance of a concrete blackbox class. 
	In contrast with the situation for \Ref{Field}s there is 
	negligible performance cost for separate compilation here.
	
	{\bf Template Parameter:} Vector - A type meeting the LinBox \Ref{VectorArchetype} interface.
	Vectors of this type are the normal arguments to {\tt apply} and {\tt applyTranspose}.
	
	@see \Ref{../archetypes} for general discussion of LinBox archetypes.
	*/

	/** \brief showing the member functions provided by all blackbox matrix classes.

	This simple interface is all that is needed for the 
blackbox algorithms.  Alternatively, the matrix archetype provides individual
matrix entry access, as needed by some algorithms, such as elimination
methods.

\ingroup blackbox
	*/
//	template <class Field>
	class BlackboxArchetype //: public BlackboxInterface 
	{
	public:

		/*- Serves in place of copy constructor.
		 * Required because constructors cannot be virtual.
		 * Make a copy of the BlackboxArchetype object.
		 * @return pointer to new blackbox object
		 */
		// clone no longer needed, since no archetype
		// virtual BlackboxArchetype* clone () const = 0;
		// Should we have clone conform more to copy construction?
		// clone(A) = make this a copy of A. -bds

	// apply variants //

		/** \brief y := Ax.  Matrix vector product. 

		The vector x must be of size A.coldim(), where A is this blackbox.
		On entry to apply, the vector y must be of size A.rowdim().
		Neither vector has it's size or capacity modified by apply.  Apply is not
		responsible for the validity of the sizes, which may or may not be checked.
		The two vectors may not overlap in memory.
		@param y it's entries are overwritten and a reference to it is also returned to allow for 
		use in nested expressions.
		@param x it's entries are the input data.
		*/
	        template <class InVector, class OutVector>
		OutVector &apply (OutVector &y, const InVector &x) const;

		/** y := Ax, using a handle for ...
		The handle serves as "protection from the future".  The idea is that the handle
		could allow the blackbox to operate more as a pure container, with the field
		(or other functionality such as dot product) provided through the handle.

		However, there are no known current uses (2003 june).  
		*/
	        template <class InVector, class OutVector>
		OutVector &apply (OutVector &y, const InVector &x, void *handle) const;

	// applyTranspose variants //

		/** \brief y := xA.
		(or from a column vector viewpoint: y := A<sup>T</sup> x.)
Matrix transpose times vector product. 

		The vector x must be of size A.rowdim(), where A is this blackbox.
		On entry to apply, the vector y must be of size A.coldim().
		Neither vector has it's size or capacity modified by applyTranspose.  ApplyTranspose is not
		responsible for the validity of the sizes, which may or may not be checked.
		The two vectors may not overlap in memory.
		@param y it's entries are overwritten and a reference to it is also returned to allow for 
		use in nested expressions.
		@param x it's entries are the input data.
		*/  
	        template <class InVector, class OutVector>
		OutVector &applyTranspose (OutVector &y, const InVector &x) const;

		/** \brief y := xA, using a handle for ...

		The handle serves as "protection from the future".  The idea is that the handle
		could allow the blackbox to operate more as a pure container, with the field
		(or other functionality such as dot product) provided through the handle.

		However, there are no known current uses (2003 june).  
		*/
	        template <class InVector, class OutVector>
		OutVector &applyTranspose (OutVector &y, const InVector &x, void *handle) const;

		/** \brief Returns the number of rows of the matrix.

		This may be zero or greater.  Currently matrix size beyond size_t is not supported.
		*/
		size_t rowdim () const;

		/** \brief Returns the number of columns of the matrix.

		This may be zero or greater.  Currently matrix size beyond size_t is not supported.
		 */
		size_t coldim () const;

#if 0
		/** \brief default constructor.
		 * Note: Most blacbbox classes will have a constructor that takes 
                 * the field and size as parameters.

		 * Developer: It is expected that each blackbox class will
		 have constructors from appropriate arguments as well.
		 Make a point to document these.
		*/
		BlackboxArchetype ();

		/** \brief copy constructor

		Subsequent modification of either source or copy does not affect the other.
		*/
		BlackboxArchetype (const BlackboxArchetype& B); 

		///\brief assignment
                BlackboxArchetype& operator=(const BlackboxArchetype& B);

		/** \brief destructor

		Releases all associated memory.
		*/

		~BlackboxArchetype();

#endif

#ifdef __LINBOX_XMLENABLED
		virtual ostream &write(ostream &) const = 0;
		virtual bool toTag(Writer &W) const = 0;
#endif

	}; // BlackBox Archetype

} // namespace LinBox

#endif // __BLACKBOX_ARCHETYPE_H










