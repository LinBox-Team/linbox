/* File: src/library/objects/blackbox/compose.h
 * Authro: William J Turner for the LinBox group
 */

#ifndef _COMPOSE_
#define _COMPOSE_

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
	class Compose : public Blackbox_archetype<Vector>
	{
	    public:

		typedef Blackbox_archetype<Vector> Blackbox;

		/** Constructor from two black box matrices.
		 * This constructor creates a matrix that is a product of two black box
		 * matrices: A*B.
		 * @param A_ptr pointer to black box matrix A.
		 * @param B_ptr pointer to black box matrix B.
		 */
		Compose (Blackbox *A_ptr, Blackbox *B_ptr)
		{
			// create new copies of matrices in dynamic memory
			if ((A_ptr != 0) && (B_ptr != 0) 
			    && (A_ptr->coldim () == B_ptr->rowdim ()))
			{
				_A_ptr = A_ptr->clone ();
				_B_ptr = B_ptr->clone ();
				_z_ptr = new(Vector);
			}
			else
				cerr << "ERROR: Cannot construct multiplication matrix." << endl;
		}

		/** Copy constructor.
		 * Creates new black box objects in dynamic memory.
		 * @param M constant reference to compose black box matrix
		 */
		Compose (const Compose<Vector>& M)
		{
			// create new copies of matrices in dynamic memory
			if ((M._A_ptr != 0) && (M._B_ptr != 0)) 
			{
				_A_ptr = M._A_ptr->clone ();
				_B_ptr = M._B_ptr->clone ();
			}
			else
				cerr << "ERROR: Cannot (copy) construct multiplication matrix." << endl;
		}

		/// Destructor
		~Compose (void)
		{
			if (_A_ptr != 0) delete _A_ptr;
			if (_B_ptr != 0) delete _B_ptr;
			delete _z_ptr;
		}

		/** Virtual constructor.
		 * Required because constructors cannot be virtual.
		 * Make a copy of the Blackbox_archetype object.
		 * Required by abstract base class.
		 * @return pointer to new blackbox object
		 */
		Blackbox* clone () const
		{ return new Compose (*this); }

		/** Application of BlackBox matrix.
		 * y = (A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		inline Vector& apply (Vector& y, const Vector& x) const
		{
			if ((_A_ptr != 0) && (_B_ptr != 0)) 
			{
				_B_ptr->apply (*_z_ptr, x);
				_A_ptr->apply (y, *_z_ptr);
			}

			return y;
		}

		/** Inplace application of BlackBox matrix.
		 * x = (A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		inline Vector& applyin (Vector& x) const
		{
			if ((_A_ptr != 0) && (_B_ptr != 0)) 
			{
				_B_ptr->applyin (x);
				_A_ptr->applyin (x);
			}

			return x;
		}

		/** Application of BlackBox matrix transpose.
		 * y= transpose(A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		inline Vector& applyTranspose (Vector& y, const Vector& x) const
		{
			if ((_A_ptr != 0) && (_B_ptr != 0)) 
			{
				_A_ptr->applyTranspose (*_z_ptr, x);
				_B_ptr->applyTranspose (y, *_z_ptr);
			}

			return y;
		}

		/** Inplace application of BlackBox matrix transpose.
		 * x = transpose(A*B)*x.
		 * Requires one vector conforming to the \Ref{LinBox}
		 * vector {@link Archetypes archetype}.
		 * Required by abstract base class.
		 * @return reference to vector y containing output.
		 * @param  x constant reference to vector to contain input
		 */
		inline Vector& applyTransposein (Vector& x) const
		{
			if ((_A_ptr != 0) && (_B_ptr != 0)) 
			{
				_A_ptr->applyTransposein (x);
				_B_ptr->applyTransposein (x);
			}

			return x;
		}
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
		size_t coldim(void) const 
		{
			if (_B_ptr != 0) 
				return _B_ptr->coldim ();
			else 
				return 0;
		}

	    private:

		// Pointers to A and B matrices
		Blackbox* _A_ptr;
		Blackbox* _B_ptr;

		// Intermediate vector
		Vector* _z_ptr;

	}; // template <Vector> class Compose

} // namespace LinBox

#endif // _COMPOSE_
