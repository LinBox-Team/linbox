/* File: src/library/archetypes/blackbox/blackbox_archetype.h
 * Author: William J Turner for the LinBox group
 */
 
#ifndef __BLACKBOX_ARCHETYPE_H
#define __BLACKBOX_ARCHETYPE_H
#include <LinBox/giverror.h>
// Namespace in which all LinBox library code resides
namespace LinBox
{

	/** BlackBox Archetype and Base Class.
	 * Found in file \URL{src/library/archetypes/blackbox/blackbox_archetype.h}.
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
	template <class Vector>
	class Blackbox_archetype 
	{
	    public:

		/** Virtual constructor.
		 * Required because constructors cannot be virtual.
		 * Make a copy of the Blackbox_archetype object.
		 * Purely virtual.
		 * @return pointer to new blackbox object
		 */
		virtual Blackbox_archetype* clone () const = 0;

		/** Application of BlackBox matrix.
		 * return A*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Purely virtual.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& apply (const Vector &x) const 
		{
			Vector *y = new Vector;

//			y->resize (rowdim ());
			return apply (*y, x);
		}

		/** Application of BlackBox matrix.
		 * y = A*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Purely virtual.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& apply (Vector &y, const Vector &x) const = 0;

		/** Application of BlackBox matrix.
		 * y = A*x.
		 * Requires two vectors conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Virtual.
		 * @return reference to y.
		 * @param  y output vector, y = Ax.  Must have size at least A.rowdim().
		 * @param  x input vector.
		 * @param  handle to protect us from the future.
		 */
		virtual Vector &apply (Vector &y, const Vector &x, void *handle) const 
		{
			if (handle == 0)
				return apply (y, x);
			else
				throw GivError ("no handle handled in this blackbox");
		}

		/** In-place application of BlackBox matrix.
		 * x = A*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Purely virtual.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& applyin (Vector &x) const 
		{
			Vector y (x);
			return apply (x, y);
		}

		/** Application of BlackBox matrix.
		 * return transpose (A)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Purely virtual.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& applyTranspose (const Vector &x) const 
		{
			Vector *y = new Vector;

//			y->resize (coldim ());
			return applyTranspose (*y, x);
		}

		/** Application of BlackBox matrix transpose.
		 * y = transpose(A)*x.
		 * Requires two vectors conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Virtual.
		 * @return reference to vector containing output.
		 * @param  y reference to vector to contain output
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& applyTranspose (Vector& y, const Vector& x) const = 0;

		/** Application of BlackBox matrix transpose.
		 * y = A*x.
		 * Requires two vectors conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Virtual.
		 * @return reference to y.
		 * @param  y output vector, y = Ax.  Must have size at least A.rowdim().
		 * @param  x input vector.
		 * @param  handle to protect us from the future.
		 */
		virtual Vector& applyTranspose (Vector& y, const Vector& x, void* handle) const 
		{
			if (handle == 0)
				return applyTranspose (y, x);
			else
				throw GivError ("no handle handled in this blackbox");
		}

		/** In-place application of BlackBox matrix tranpose.
		 * x = tranpose (A)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Purely virtual.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		virtual Vector& applyTransposein (Vector &x) const 
		{
			Vector y (x);
			return applyTranspose (x, y);
		}

		/** Retreive row dimensions of BlackBox matrix.
		 * This may be needed for applying preconditioners.
		 * Purely virtual.
		 * @return integer number of rows of black box matrix.
		 */
		virtual size_t rowdim (void) const = 0;

		/** Retreive column dimensions of BlackBox matrix.
		 * Purely virtual.
		 * @return integer number of columns of black box matrix.
		 */
		virtual size_t coldim (void) const = 0;

	    protected:

		/** Default constructor.
		 * The default constructor is required by derived classes, but because 
		 * this class contains purely virtual functions, it should never be
		 * called without a derived class.
		 */
		Blackbox_archetype (void) {}
    
	}; // BlackBox Archetype

} // namespace LinBox

#endif // __BLACKBOX_ARCHETYPE_H
