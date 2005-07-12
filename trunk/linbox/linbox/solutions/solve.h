/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/solutions/solve.h
 *  Evolved from an earlier one by Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * See COPYING for license information.
 */

#ifndef __SOLVE_H
#define __SOLVE_H

#include <vector>
#include <algorithm>

// must fix this list...
#include "linbox/algorithms/wiedemann.h"
#include "linbox/algorithms/lanczos.h"
#include "linbox/algorithms/block-lanczos.h"
#include "linbox/algorithms/rational-solver.h"
#include "linbox/algorithms/diophantine-solver.h"
#include "linbox/blackbox/dense.h"
#include "linbox/matrix/factorized-matrix.h"
#include "linbox/util/debug.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/solutions/methods.h"
#include "linbox/algorithms/bbsolve.h"

namespace LinBox 
{

	// for specialization with respect to the DomainCategory
	template< class Vector, class Blackbox, class SolveMethod, class DomainCategory>
	Vector & solve (Vector & 			x,
			const Blackbox &                A,
			const Vector &			b,
			const DomainCategory &        tag,
			const SolveMethod &            M);
	//		SolveStatus * 			s = 0);

	/** \brief Solve Ax = b, for x.
	 *
	 * Vector x such that Ax = b is returned.  
	 In the case of a singular matrix A, if the system is consistent, a random
	 solution is returned by default.  The method parameter may contain
	 an indication that an arbitrary element of the solution space is 
	 acceptable, which can be faster to compute.  
	 If the system is inconsistent the zero vector is returned. 
	 
         \ingroup solutions
        */
	//and the SolveStatus, if non-null, is set to indicate inconsistency.
	template< class Vector, class Blackbox, class SolveMethod>
	Vector & solve (Vector &        		x,
			const Blackbox &                A,
			const Vector &			b,
			const SolveMethod &             M)
	//		SolveStatus * 			s = 0)
	{ 
		return solve(x, A, b, typename FieldTraits<typename Blackbox::Field>::categoryTag(), M);
	}

	// the solve with default method
	template< class Vector, class Blackbox>
	Vector& solve(Vector& x, const Blackbox& A, const Vector& b)
	{ return solve(x, A, b, Method::Hybrid()); }

	// in methods.h FoobarMethod and Method::Foobar are the same class.
	// in methods.h template<BB> bool useBB(const BB& A) is defined.

	// specialize this on blackboxes which have local methods
	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const Method::Hybrid& m)
	{	
		if (useBB(A)) return solve(x, A, b, Method::Blackbox(m)); 
		else return solve(x, A, b, Method::Elimination(m));
	}

	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const Method::Blackbox& m)
	{ 
		// what is chosen here should be best and/or most reliable currently available choice
		//	return solve(x, A, b, Method::Lanczos(m));
		// maybe should be:
		return solve(x, A, b, Method::BlockLanczos(m));
	}

	// temporary - fix this
#define inBlasRange(p) true

	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const Method::Elimination& m)
	{ 
		integer c, p;
		A.field().cardinality(c);
		A.field().characteristic(p);
		//if ( p == 0 || (c == p && inBlasRange(p)) )
		return solve(x, A, b, 
			     typename FieldTraits<typename BB::Field>::categoryTag(), 
			     Method::BlasElimination(m)); 
  		//else 
		//	return solve(x, A, b, 
		//			typename FieldTraits<typename BB::Field>::categoryTag(), 
		//			Method::NonBlasElimination(m)); 
	}

	template <class Vector, class Field> 
	Vector& solve(Vector& x, const SparseMatrix<Field>& A, const Vector& b, 
		      const Method::Elimination& m)
	{	bool consistent = false;
		// sparse elimination based solver can be called here ?
        	// For now we call the dense one
        	
		return solve(x, A, b, 
			     typename FieldTraits<typename SparseMatrix<Field>::Field>::categoryTag(), 
			     Method::BlasElimination(m)); 

// 		if ( ! consistent ) {  // we will return the zero vector
// 			typename Field::Element zero; A.field().init(zero, 0);
// 			for (typename Vector::iterator i = x.begin(); i != x.end(); ++i) *i = zero;
// 		}
// 		return x;
	}
	// BlasElimination section ///////////////////

	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		BlasBlackbox<typename BB::Field> B(A); // copy A into a BlasBlackbox
		return solve(x, B, b, tag, m);
	} 

	template <class Vector, class Field> 
	Vector& solve(Vector& x, const BlasBlackbox<Field>& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		bool consistent = false;
		LQUPMatrix<Field> LQUP(A);
		//FactorizedMatrix<Field> LQUP(A);

		LQUP.left_solve(x, b);

		// this should be implemented directly in left_solve 
		//if ( ! consistent ) {  // we will return the zero vector
		//	typename Field::Element zero; A.field().init(zero, 0);
		//	for (typename Vector::iterator i = x.begin(); i != x.end(); ++i) *i = zero;
		//}
		return x;
	} 

	
	/* Integer tag Specialization for Dixon method:
	 * 2 interfaces: 
	 *   - the output is a common denominator and a vector of numerator (no need of rational number)
	 *   - the output is a vector of rational
	 */  





	// error handler for bad use of the integer solver API
	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		std::cout<<"try to solve system over the integer\n"
			 <<"the API need either \n"
			 <<" - a vector of rational as the solution \n"
			 <<" - or an integer for the common denominator and a vector of integer for the numerators\n\n";
		LinboxError("bad use of integer API solver\n");
		
	} 


	
	/*
	 * 1st integer solver API :
	 * solution is a vector of rational numbers
	 * RatVector is assumed to be the type of a vector of rational number
	 */
	
	// default API (methos is BlasElimination)
	template<class RatVector, class Vector, class BB>	
	RatVector& solve(RatVector& x, const BB &A, const Vector &b){
		return solve(x, A, b, Method::BlasElimination());
	}

	// launcher of specialized solver depending on the MethodTrait
	template<class RatVector, class Vector, class BB, class MethodTraits>	
	RatVector& solve(RatVector& x, const BB &A, const Vector &b, const MethodTraits &m){
		return solve(x, A, b, typename FieldTraits<typename BB::Field>::categoryTag(),  m);
	}

	
	/* Specializations for BlasElimination over the integers
	 */

	// input matrix is generic (copying it into a BlasBlackbox)
	template <class RatVector, class Vector, class BB> 
	Vector& solve(RatVector& x, const BB& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		BlasBlackbox<typename BB::Field> B(A); // copy A into a BlasBlackbox
		return solve(x, B, b, tag, m);
	} 
	
	// input matrix is a BlasBlackbox (no copy)
	template <class RatVector, class Vector, class Ring> 
	Vector& solve(RatVector& x, const BlasBlackbox<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		Method::Dixon mDixon(m);
		typename Ring::Element d;
		std::vector< typename Ring::Element> num(A.coldim());
		solve (d, num, A, b, tag, mDixon);
		typename RatVector::iterator it_x= x.begin();
		typename std::vector< typename Ring::Element>::iterator it_num= num.begin();
		for (; it_x != x.end(); ++it_x, ++it_num){			
			*it_x = typename RatVector::value_type(*it_num, d);
		}
			
		return x;
	} 

	// input matrix is a DenseMatrix (no copy)
	template <class RatVector, class Vector, class Ring> 
	Vector& solve(RatVector& x, const DenseMatrix<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		Method::Dixon mDixon(m);
		typename Ring::Element d;
		std::vector< typename Ring::Element> num(A.coldim());
		solve (d, num, A, b, tag, mDixon);
		typename RatVector::iterator it_x= x.begin();
		typename std::vector< typename Ring::Element>::iterator it_num= num.begin();
		for (; it_x != x.end(); ++it_x, ++it_num){			
			*it_x = typename RatVector::value_type(*it_num, d);
		}
		
		return x;
	} 

	/*
	 * 2nd integer solver API :
	 * solution is a formed by a common denominator and a vector of integer numerator
	 * solution is num/d
	 */
	
	
	// default API (methid is BlasElimination)
	template< class Vector, class BB>
	Vector& solve(typename BB::Field::Element &d, Vector &x, const BB &A, const Vector &b){
		return solve(d, x, A, b, typename FieldTraits<typename BB::Field>::categoryTag(),  Method::BlasElimination());
	}
		
	// launcher of specialized solver depending on the MethodTraits
	template< class Vector, class BB, class MethodTraits>
	Vector& solve(typename BB::Field::Element &d, Vector &x, const BB &A, const Vector &b, const MethodTraits &m){
		return solve(d, x, A, b, typename FieldTraits<typename BB::Field>::categoryTag(), m);
	}
	
	/* Specialization for BlasElimination over the integers
	 */
	
	// input matrix is generic (copying it into a BlasBlackbox)
	template <class Vector, class BB> 
	Vector& solve(typename BB::Field::Element &d, Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		BlasBlackbox<typename BB::Field> B(A); // copy A into a BlasBlackbox
		return solve(d, x, B, b, tag, m);
	} 
	
	// input matrix is a BlasBlackbox (no copy)
	template <class Vector, class Ring> 
	Vector& solve(typename Ring::Element &d, Vector& x, const BlasBlackbox<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		Method::Dixon mDixon(m);
		return solve(d, x, A, b, tag, mDixon);
	} 

	// input matrix is a DenseMatrix (no copy)
	template <class Vector, class Ring> 
	Vector& solve(typename Ring::Element &d, Vector& x, const DenseMatrix<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag & tag, 
		      const Method::BlasElimination& m)
	{ 
		Method::Dixon mDixon(m);
		return solve(d, x, A, b, tag, mDixon);
	}



	/** \brief solver specialization with the 2nd API and DixonTraits over integer (no copying)
	 */
	template <class Vector, class Ring> 
	Vector& solve(typename Ring::Element &d, Vector& x, const BlasBlackbox<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag tag, Method::Dixon& m)
	{ 
		linbox_check ((x.size () == A.coldim ()) && (b.size () == A.rowdim ()));
		commentator.start ("Padic Integer Blas-based Solving", "solving");
		
		typedef Modular<double> Field;
		// 0.7213475205 is an upper approximation of 1/(2log(2))
		RandomPrime genprime( 26-(int)ceil(log((double)A.rowdim())*0.7213475205)); 
		RationalSolver<Ring, Field, RandomPrime, DixonTraits> rsolve(A.field(), genprime); 			
		SolverReturnStatus status;

		// if singularity unknown and matrix is square, we try nonsingular solver
		switch ( m.singular() ) {
		case Specifier::SINGULARITY_UNKNOWN:
			switch (A.rowdim() == A.coldim() ? 
				status=rsolve.solveNonsingular(x, d, A, b, false ,m.maxTries()) : SS_SINGULAR) {				
			case SS_OK:
				m.singular(Specifier::NONSINGULAR);				
				break;					
			case SS_SINGULAR:
				switch (m.solution()){
				case DixonTraits::DETERMINIST:
					status= rsolve.monolithicSolve(x, d, A, b, false, false, m.maxTries(), 
								       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
					break;					
				case DixonTraits::RANDOM:
					status= rsolve.monolithicSolve(x, d, A, b, false, true, m.maxTries(), 
								       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
					break;					
				case DixonTraits::DIOPHANTINE:
					{ 
                                            DiophantineSolver<RationalSolver<Ring,Field,RandomPrime, DixonTraits> > dsolve(rsolve);
                                            status= dsolve.diophantineSolve(x, d, A, b, m.maxTries(),
                                                                            (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
                                        }
                                        break;					
				default:
					break;
				}			
				break;
			}
			
		case Specifier::NONSINGULAR:
			rsolve.solveNonsingular(x, d, A, b, false ,m.maxTries());
			break;
			    
		case Specifier::SINGULAR:
			switch (m.solution()){
			case DixonTraits::DETERMINIST:
				status= rsolve.monolithicSolve(x, d, A, b, 
							       false, false, m.maxTries(), 
							       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
				break;
				
			case DixonTraits::RANDOM:
				status= rsolve.monolithicSolve(x, d, A, b, 
							       false, true, m.maxTries(), 
							       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
				break;
				
			case DixonTraits::DIOPHANTINE:
				{
                                    DiophantineSolver<RationalSolver<Ring,Field,RandomPrime, DixonTraits> > dsolve(rsolve);
                                    status= dsolve.diophantineSolve(x, d, A, b, m.maxTries(),
                                                                    (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
                                }
				break;
				
			default:
				break;
			}		
		default:			    
			break;
		}

		
		if ( status == SS_INCONSISTENT ) {  // we will return the zero vector
			typename Ring::Element zero; A.field().init(zero, 0);
			for (typename Vector::iterator i = x.begin(); i != x.end(); ++i) *i = zero;
		}
		commentator.stop("done", NULL, "solving");

		return x;
	}	

	/** \brief solver specialization with the 2nd API and DixonTraits over integer (no copying)
	 */
	template <class Vector, class Ring> 
	Vector& solve(typename Ring::Element &d, Vector& x, const DenseMatrix<Ring>& A, const Vector& b, 
		      const RingCategories::IntegerTag tag, const Method::Dixon& m)
	{  
		// NOTE: righ now return only the numerator of the rational solution
		//       NEED TO BE FIXED !!!

		linbox_check ((x.size () == A.coldim ()) && (b.size () == A.rowdim ()));
		commentator.start ("Padic Integer Blas-based Solving", "solving");
		
		typedef Modular<double> Field;
		// 0.7213475205 is an upper approximation of 1/(2log(2))
		RandomPrime genprime( 26-(int)ceil(log((double)A.rowdim())*0.7213475205)); 
		RationalSolver<Ring, Field, RandomPrime, DixonTraits> rsolve(A.field(), genprime); 			
		SolverReturnStatus status;

		// if singularity unknown and matrix is square, we try nonsingular solver
		switch ( m.singular() ) {
		case Specifier::SINGULARITY_UNKNOWN:
			switch (A.rowdim() == A.coldim() ? 
				status=rsolve.solveNonsingular(x, d, A, b, false ,m.maxTries()) : SS_SINGULAR) {				
			case SS_OK:
				m.singular(Specifier::NONSINGULAR);				
				break;					
			case SS_SINGULAR:
				switch (m.solution()){
				case DixonTraits::DETERMINIST:
					status= rsolve.monolithicSolve(x, d, A, b, false, false, m.maxTries(), 
								       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
					break;					
				case DixonTraits::RANDOM:
					status= rsolve.monolithicSolve(x, d, A, b, false, true, m.maxTries(), 
								       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
					break;					
				case DixonTraits::DIOPHANTINE:
					DiophantineSolver<RationalSolver<Ring,Field,RandomPrime, DixonTraits> > dsolve(rsolve);
					status= dsolve.diophantineSolve(x, d, A, b, m.maxTries(),
									(m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
					break;					
				default:
					break;
				}			
				break;
			}
			
		case Specifier::NONSINGULAR:
			rsolve.solveNonsingular(x, d, A, b, false ,m.maxTries());
			break;
			    
		case Specifier::SINGULAR:
			switch (m.solution()){
			case DixonTraits::DETERMINIST:
				status= rsolve.monolithicSolve(x, d, A, b, 
							       false, false, m.maxTries(), 
							       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
				break;
				
			case DixonTraits::RANDOM:
				status= rsolve.monolithicSolve(x, d, A, b, 
							       false, true, m.maxTries(), 
							       (m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
				break;
				
			case DixonTraits::DIOPHANTINE:
				DiophantineSolver<RationalSolver<Ring,Field,RandomPrime, DixonTraits> > dsolve(rsolve);
				status= dsolve.diophantineSolve(x, d, A, b, m.maxTries(),
								(m.certificate()? SL_LASVEGAS: SL_MONTECARLO));
				break;
				
			default:
				break;
			}		
		default:			    
			break;
		}

		
		if ( status == SS_INCONSISTENT ) {  // we will return the zero vector
			typename Ring::Element zero; A.field().init(zero, 0);
			for (typename Vector::iterator i = x.begin(); i != x.end(); ++i) *i = zero;
		}
		commentator.stop("done", NULL, "solving");
		return x;	
	}	

	/*
	  struct BlasEliminationCRASpecifier;
	  // Extra case put in (1) for timing comparison or (2) for parallelism or 
	  // (3) as an example of how we might leave an abandoned choice around in a 
	  // callable state for future reference 
	  template <class Vector, class Field> 
	  Vector& solve(Vector& x, const DenseMatrix<Field>& A, const Vector& b, 
	  const RingCategories::IntegerTag & tag, 
	  const BlasEliminationCRASpecifier & m)
	  { // (low priority) J-G puts in code using CRA object CRA and solve(x, A, b, ModularTag, Method::BlasElimination) 
	  typename Field::Element zero; A.field().init(zero, 0);
	  for (typename Vector::iterator i = x.begin(); i != x.end(); ++i) *i = zero;
	  return x;
	  } 
	*/

	// NonBlasElimination section ////////////////

	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::NonBlasElimination& m)
	{	DenseMatrix<typename BB::Field> B(A); // copy
		return solve(x, B, b, tag, m);
	}

	// specialization when no need to copy
	template <class Vector, class Field> 
	Vector& solve(Vector& x, const DenseMatrix<Field>& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::NonBlasElimination& m)
	{ //Do we have a non blas elimination?  There was not one in the original solve.h (now algorithms/bbsolve.h).
		return x;
	}

	// note: no need for NonBlasElimination when RingCategory is integer

	// Lanczos ////////////////
	// may throw SolverFailed or InconsistentSystem
	
	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::Lanczos& m)
	{
		solve(A, x, b, A.field(), m);
		return x;
	}



	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::BlockLanczos& m)
	{
            try { 
                solve(A, x, b, A.field(), m); 
            } catch (SolveFailed) {
                typename BB::Field::Element zero; A.field().init(zero, 0);
                for (typename Vector::iterator i = x.begin(); 
                     i != x.end(); ++i) 
                    *i = zero;
            }
            return x;
	}

	// Wiedemann section ////////////////

	// may throw SolverFailed or InconsistentSystem
	template <class Vector, class BB> 
	Vector& solve(Vector& x, const BB& A, const Vector& b, 
		      const RingCategories::ModularTag & tag, 
		      const Method::Wiedemann& m)
	{
		// adapt to earlier signature of wiedemann solver
		solve(A, x, b, A.field(), m);
		return x;
	}


	/* remark 1.  I used copy constructors when switching method types.
	   But if the method types are (empty) child classes of a common  parent class containing
	   all the information, then casts can be used in place of copies.
	*/ 

} // LinBox



#include "linbox/field/modular.h"
#include "linbox/algorithms/cra-domain.h"
#include "linbox/randiter/random-prime.h"
#include "linbox/algorithms/matrix-hom.h"
#include "linbox/vector/vector-traits.h"

namespace LinBox {
   

    template <class Blackbox, class Vector, class MyMethod>
    struct IntegerModularSolve {       
        const Blackbox &A;
        const Vector &B;
        const MyMethod &M;

        IntegerModularSolve(const Blackbox& b, const Vector& v, const MyMethod& n) 
                : A(b), B(v), M(n) {}
        
        
        template<typename Field>
        typename Rebind<Vector, Field>::other& operator()(typename Rebind<Vector, Field>::other& x, const Field& F) const {
            typedef typename Blackbox::template rebind<Field>::other FBlackbox;
            FBlackbox * Ap;
            MatrixHom::map(Ap, A, F);

            typedef typename Rebind<Vector, Field>::other FVector;
            Hom<typename Blackbox::Field, Field> hom(A.field(), F);
            FVector Bp(B.size());
            typename Vector::const_iterator Bit = B.begin();
            typename FVector::iterator      Bpit = Bp.begin();
            for( ; Bit != B.end(); ++Bit, ++Bpit)
                hom.image (*Bpit, *Bit);

            VectorWrapper::ensureDim (x, A.coldim());
            solve( x, *Ap, Bp, RingCategories::ModularTag(), M);
            delete Ap;

            return x;
        }            
    };

	// may throw SolverFailed or InconsistentSystem
    template <class Vector, class BB, class MyMethod> 
    Vector& solve(Vector& x, const BB& A, const Vector& b, 
                  const RingCategories::IntegerTag & tag, 
                  const MyMethod& M)
    {
        commentator.start ("Integer CRA Solve", "Isolve");
        RandomPrime genprime( 26 ); 
        ChineseRemainder< Modular<double> > cra(3UL,A.coldim());
        IntegerModularSolve<BB,Vector,MyMethod> iteration(A, b, M);
        cra(x, iteration, genprime);
        commentator.stop ("done", NULL, "Isolve");
        return x;
    }
    
} // LinBox




#endif // __SOLVE_H

