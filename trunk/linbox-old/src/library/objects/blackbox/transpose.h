/* File: src/library/objects/blackbox/compose.h
 * Authro: William J Turner for the LinBox group
 */

#ifndef _MULTIPLY_
#define _MULTIPLY_

#include "LinBox/blackbox_archetype.h"

// Namespace in which all LinBox library code resides
namespace LinBox
{

	/** Blackbox compose matrix.
	 * This is a class that multiplies two matrices by implementing an 
	 * apply method that calls the apply methods of both of the consituent 
	 * matrices.
	 *
	 * This class, like the Black Box archetype from which it is derived, 
	 * is templatized by the vector type to which the matrix is applied.  
	 * Both constituent matrices must also use this same vector type.
	 * @param Vector \Ref{LinBox} dense or sparse vector of field elements
	 */
	template <class Vector>
	class Transpose : public Blackbox_archetype<Vector>
	{
	public:

		typedef Blackbox_archetype<Vector> Blackbox;

		/** Constructor from a black box.
		 * This constructor creates a matrix that the transpose of a black box
		 * matrix A
		 * @param A_ptr pointer to black box matrix.
		 */
		Transpose (Blackbox *A_ptr)
		{
			// create new copies of matrices in dynamic memory
			if (A_ptr != 0)
				_A_ptr = A_ptr->clone ();
			else
				cerr << "ERROR: Cannot construct multiplication matrix." << endl;
		}

		/** Copy constructor.
		 * Creates new black box objects in dynamic memory.
		 * @param M constant reference to compose black box matrix
		 */
		Transpose (const Compose<Vector> &M)
		{
			// create new copies of matrices in dynamic memory
			if (M._A_ptr != 0)
				_A_ptr = M._A_ptr->clone ();
			else
				cerr << "ERROR: Cannot (copy) construct transpose matrix." << endl;
		}

		/// Destructor
		~Transpose (void)
		{
			if (_A_ptr != 0) delete _A_ptr;
		}

		/** Virtual constructor.
		 * Required because constructors cannot be virtual.
		 * Make a copy of the Blackbox_archetype object.
		 * Required by abstract base class.
		 * @return pointer to new blackbox object
		 */
		Blackbox *clone () const
			{ return new Transpose (*this); }

		/** Application of BlackBox matrix.
		 * y= (A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		inline Vector &apply (Vector &y, const Vector &x) const
		{
			if (_A_ptr != 0)
				_A_ptr->applyTranspose (y, x);

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
		inline Vector &applyTranspose (Vector &y, const Vector &x) const
		{
			if (_A_ptr != 0)
				_A_ptr->apply (y, x);

			return y;
		}

		/** Retreive row dimensions of BlackBox matrix.
		 * This may be needed for applying preconditioners.
		 * Required by abstract base class.
		 * @return integer number of rows of black box matrix.
		 */
		size_t rowdim (void) const
		{
			if (_A_ptr != 0) 
				return _A_ptr->coldim ();
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
				return _A_ptr->rowdim ();
			else 
				return 0;
		}

	private:

		// Pointers to A and B matrices
		Blackbox *_A_ptr;

	}; // template <Vector> class Compose

} // namespace LinBox

#endif // _MULTIPLY_
