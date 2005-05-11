/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/algorithms/wiedemann.inl
 * Copyright (C) 2002 Zhendong Wan
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Zhendong Wan <wan@mail.eecis.udel.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * ------------------------------------
 * 2002-10-02  Bradford Hovinen  <bghovinen@math.uwaterloo.ca>
 *
 * Refactoring:
 * Put everything inside a WiedemannSolver class, with the following
 * interface:
 *    solve - Solve a general linear system
 *    solveNonsingular - Solve a nonsingular system
 *    solveSingular - Solve a general singular system
 *    findRandomSolution - Find a random solution to a singular preconditioned
 *                         problem
 *    findNullspaceElement - Find an element of the right nullspace
 *    certifyInconsistency - Find a certificate of inconsistency for a
 *                           linear system
 *    precondition - Form a preconditioner and return it
 * ------------------------------------
 * 2002-08-09  Bradford Hovinen  <hovinen@cis.udel.edu>
 *
 * Move the Wiedemann stuff over to this file
 *
 * Create a singular and nonsingular version that is a bit intelligent about
 * which one to use in different circumstances
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __WIEDEMANN_INL
#define __WIEDEMANN_INL

#include <vector>
#include <algorithm>

#include "linbox/algorithms/wiedemann.h"
#include "linbox/blackbox/submatrix.h"
#include "linbox/blackbox/butterfly.h"
#include "linbox/blackbox/transpose.h"
#include "linbox/algorithms/blackbox-container.h"
#include "linbox/algorithms/blackbox-container-symmetric.h"
#include "linbox/algorithms/massey-domain.h" 
#include "linbox/switch/cekstv.h"
#include "linbox/solutions/rank.h"
#include "linbox/vector/stream.h"

namespace LinBox 
{

template <class Field, class Vector>
typename WiedemannSolver<Field, Vector>::ReturnStatus
WiedemannSolver<Field, Vector>::solve (const BlackboxArchetype&A,
				       Vector &x,
				       const Vector &b,
				       Vector &u)
{
	linbox_check ((x.size () == A.coldim ()) &&
		      (b.size () == A.rowdim ()));
	linbox_check (_traits.singular () != WiedemannTraits::NONSINGULAR || A.coldim () == A.rowdim ());

	commentator.start ("Solving linear system (Wiedemann)", "WiedemannSolver::solve");

	WiedemannTraits::SingularState singular = _traits.singular ();
	ReturnStatus status = FAILED;

	unsigned int tries = _traits.maxTries ();

	unsigned long r = (unsigned long) -1;

	// Dan Roche 6-21-04 Changed this from UNKNOWN which I think was incorrect
	if (_traits.rank () != WiedemannTraits::RANK_UNKNOWN)
		r = _traits.rank ();

	while (status == FAILED && tries-- > 0) {
		switch (singular) {
		    case WiedemannTraits::UNKNOWN:
		    {
			switch (solveNonsingular (A, x, b, true)) {
			    case OK:
				status = OK;
				break;

			    case FAILED:
				break;

			    case SINGULAR:
				commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
					<< "System found to be singular. Reverting to nonsingular solver." << std::endl;
				singular = WiedemannTraits::SINGULAR;
				break;

			    default:
				throw LinboxError ("Bad return value from solveNonsingular");
			}
			break;
		    }

		    case WiedemannTraits::NONSINGULAR:
		    {
			switch (solveNonsingular (A, x, b, false)) {
			    case OK:
				status = OK;
				break;

			    case FAILED:
				break;

			    case SINGULAR:
				status = SINGULAR;
				break;

			    default:
				throw LinboxError ("Bad return value from solveNonsingular");
			}

			break;
		    }

		    case WiedemannTraits::SINGULAR:
		    {
			if (r == (unsigned long) -1) {
				rank (r, A, _F);
				commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
					<< "Rank of A = " << r << std::endl;
			}

			switch (solveSingular (A, x, b, u, r)) {
			    case OK:
				status = OK;
				break;

			    case FAILED:
				r = (unsigned long) -1;
				break;

			    case SINGULAR:
				throw LinboxError ("solveSingular returned SINGULAR");

			    default:
				break;

			    case INCONSISTENT:
				status = INCONSISTENT;
			}

			break;
		    }
		}
	}

	if (status == FAILED)
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Maximum tries exceeded with no resolution. Giving up." << std::endl;

	commentator.stop ("done", NULL, "WiedemannSolver::solve");

	return status;
}

template <class Field, class Vector>
typename WiedemannSolver<Field, Vector>::ReturnStatus
WiedemannSolver<Field, Vector>::solveNonsingular (const BlackboxArchetype&A,
						  Vector &x,
						  const Vector &b,
						  bool useRandIter)
{
	typedef std::vector<typename Field::Element> Polynomial;
	typedef typename Polynomial::iterator        PolyIterator;

	commentator.start ("Solving nonsingular system (Wiedemann)", "WiedemannSolver::solveNonsingular");

	Polynomial m_A;
	Vector     z;
	bool       ret = false;

	{
		commentator.start ("Computing minimal polynomial");

		unsigned long  deg;

		if (!_traits.symmetric ()) {
			typedef BlackboxContainer<Field, Vector> BBContainer;

			if (useRandIter) {
				BBContainer                      TF (&A, _F, _randiter);
				MasseyDomain<Field, BBContainer> WD (&TF);

				WD.minpoly (m_A, deg);
			} else {
				BBContainer                      TF (&A, _F, b);
				MasseyDomain<Field, BBContainer> WD (&TF);

				WD.minpoly (m_A, deg);
			}
		} else {
			typedef BlackboxContainerSymmetric<Field, Vector> BBContainer;

			if (useRandIter) {
				BBContainer                      TF (&A, _F, _randiter);
				MasseyDomain<Field, BBContainer> WD (&TF);

				WD.minpoly (m_A, deg);
			} else {
				BBContainer                      TF (&A, _F, b);
				MasseyDomain<Field, BBContainer> WD (&TF);

				WD.minpoly (m_A, deg);
			}
		}

		commentator.stop ("done");
	}

	std::ostream &report = commentator.report (Commentator::LEVEL_UNIMPORTANT, INTERNAL_DESCRIPTION);
	report << "Minimal polynomial coefficients: ";
	_VD.write (report, m_A) << std::endl;

	if (_F.isZero (m_A.front ())) {
		commentator.stop ("singular", "System found to be singular",
				  "WiedemannSolver::solveNonsingular");
		return SINGULAR;
	}

	{
		commentator.start ("Preparing polynomial for application");

		PolyIterator iter = m_A.begin ();

		while (++iter != m_A.end ()) {
			_F.divin (*iter, m_A.front ());
			_F.negin (*iter);
		}

		commentator.stop ("done");
	}

	{
		commentator.start ("Applying polynomial via Horner's rule", NULL, m_A.size () - 1);

		_VD.mul (x, b, m_A.back ());

		VectorWrapper::ensureDim (z, A.rowdim ());

		for (int i = m_A.size () - 1; --i > 0;) {
			if ((m_A.size () - i) & 0xff == 0)
				commentator.progress (m_A.size () - i);

			A.apply (z, x);
			_VD.axpy (x, m_A[i], b, z);
		}

		commentator.stop ("done");
	}

	if (_traits.checkResult ()) {
		commentator.start ("Checking whether Ax=b");
		A.apply (z, x);

		if (_VD.areEqual (z, b))
			ret = true;
		else
			ret = false;

		commentator.stop (MSG_STATUS (ret));
	}

	commentator.stop (MSG_STATUS (ret), NULL, "WiedemannSolver::solveNonsingular");

	if (!ret)
		return FAILED;

	return OK;
}

template <class Field, class Vector>
typename WiedemannSolver<Field, Vector>::ReturnStatus
WiedemannSolver<Field, Vector>::solveSingular (const BlackboxArchetype&A,
					       Vector &x,
					       const Vector &b,
					       Vector &u,
					       unsigned long r)
{
	commentator.start ("Solving singular system (Wiedemann)", "WiedemannSolver::solveSingular");

	Vector Ax;
	ReturnStatus status = OK;

	BlackboxArchetype*P = NULL;
	BlackboxArchetype*Q = NULL;
	BlackboxArchetype*PAQ = NULL;
	const BlackboxArchetype*B = precondition (A, PAQ, P, Q);

	switch (findRandomSolution (*B, x, b, r, P, Q)) {
	    case BAD_PRECONDITIONER:
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Preconditioned matrix did not have generic rank profile" << std::endl;

		status = BAD_PRECONDITIONER;
		break;

	    case FAILED:
		if (_traits.certificate ()) {
			VectorWrapper::ensureDim (u, A.rowdim ());

			if (certifyInconsistency (u, A, b))
				status = INCONSISTENT;
			else
				status = FAILED;
		} else
			status = FAILED;

		break;

	    case SINGULAR:
		throw LinboxError ("findRandomSolution returned SINGULAR");

	    case INCONSISTENT:
		throw LinboxError ("findRandomSolution returned INCONSISTENT");

	    case OK:
		break;
	}

	if (status == OK && _traits.checkResult ()) {
		commentator.start ("Checking system solution");

		VectorWrapper::ensureDim (Ax, A.rowdim ());

		A.apply (Ax, x);

		if (_VD.areEqual (Ax, b))
			commentator.stop ("passed");
		else {
			commentator.stop ("FAILED");

			if (_traits.certificate ()) {
				commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
					<< "Computed system solution is not correct. "
					<< "Attempting to find certificate of inconsistency." << std::endl;

				VectorWrapper::ensureDim (u, A.rowdim ());

				if (certifyInconsistency (u, A, b))
					status = INCONSISTENT;
				else
					status = FAILED;
			} else
				status = FAILED;
		}
	}

	if (PAQ != NULL) delete PAQ;
	if (P != NULL) delete P;
	if (Q != NULL) delete Q;

	commentator.stop ("done", NULL, "WiedemannSolver::solveSingular");

	return status;
}

template <class Field, class Vector>
typename WiedemannSolver<Field, Vector>::ReturnStatus
WiedemannSolver<Field, Vector>::findRandomSolution (const BlackboxArchetype&A,
						    Vector                          &x,
						    const Vector                    &b,
						    size_t                           r,
						    const BlackboxArchetype*P,
						    const BlackboxArchetype*Q)
{
	commentator.start ("Solving singular system with generic rank profile (Wiedemann)",
			   "WiedemannSolver::findRandomSolution");

	Vector v, Avpb, PAvpb, bp, xp, Qinvx;

	RandomDenseStream<Field, Vector> stream (_F, _randiter, A.coldim ());

	VectorWrapper::ensureDim (v, A.coldim ());
	VectorWrapper::ensureDim (Avpb, A.rowdim ());
	VectorWrapper::ensureDim (xp, r);
	VectorWrapper::ensureDim (bp, r);

	{
		commentator.start ("Preparing right hand side");

		stream >> v;

		A.apply (Avpb, v);
		_VD.addin (Avpb, b);

		if (P != NULL) {
			VectorWrapper::ensureDim (PAvpb, A.rowdim ());
			P->apply (PAvpb, Avpb);
			_VD.copy (bp, PAvpb, 0, r);
		} else {
			_VD.copy (bp, Avpb, 0, r);
		}

		commentator.stop ("done");
	}

	Submatrix<Field, Vector> Ap (_F, &A, 0, 0, r, r);

	switch (solveNonsingular (Ap, xp, bp, false)) {
	    case SINGULAR:
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Leading principal minor was found to be singular." << std::endl;
		commentator.stop ("bad preconditioner", "System was not well-conditioned",
				  "WiedemannSolver::findRandomSolution");
		return BAD_PRECONDITIONER;

	    case OK:
		break;

	    default:
		throw LinboxError ("solveNonsingular returned bad value");
	}

	if (Q != NULL) {
		VectorWrapper::ensureDim (Qinvx, A.coldim ());
		_VD.copy (Qinvx, xp);
		Q->apply (x, Qinvx);
	} else {
		_VD.copy (x, xp);
	}

	_VD.subin (x, v);

	commentator.stop ("done", NULL, "WiedemannSolver::findRandomSolution");

	return OK;
}

template <class Field, class Vector>
typename WiedemannSolver<Field, Vector>::ReturnStatus
WiedemannSolver<Field, Vector>::findNullspaceElement (Vector                          &x,
						      const BlackboxArchetype&A)
{
	commentator.start ("Finding a nullspace element (Wiedemann)", "WiedemannSolver::findNullspaceElement");

	Vector v, Av, PAv, vp, xp, Qinvx;

	RandomDenseStream<Field, Vector> stream (_F, _randiter, A.coldim ());

	unsigned long r = (A.coldim () < A.rowdim ()) ? A.coldim () : A.rowdim ();

	VectorWrapper::ensureDim (v, A.coldim ());
	VectorWrapper::ensureDim (Av, A.rowdim ());

	ReturnStatus status;

	{
		commentator.start ("Constructing right hand side");

		stream >> v;
		A.apply (Av, v);

		if (A.coldim () < A.rowdim ()) {
			VectorWrapper::ensureDim (vp, r);
			_VD.copy (vp, Av, 0, r);
		}

		commentator.stop ("done");
	}

	if (A.coldim () < A.rowdim ()) {
		Submatrix<Field, Vector> Ap (_F, &A, 0, 0, r, r);
		status = solveNonsingular (Ap, x, vp, false);
	}
	else if (A.rowdim () < A.coldim ()) {
		Submatrix<Field, Vector> Ap (_F, &A, 0, 0, r, r);
		VectorWrapper::ensureDim (xp, r);
		status = solveNonsingular (Ap, xp, Av, false);
		_VD.copy (x, xp);
	} else
		status = solveNonsingular (A, x, Av, false);

	if (status == SINGULAR) {
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Leading principal minor was found to be singular." << std::endl;
		commentator.stop ("bad preconditioner", "System not well-conditioned",
				  "WiedemannSolver::findNullspaceElement");
		return BAD_PRECONDITIONER;
	}

	_VD.subin (x, v);

	commentator.stop ("done", NULL, "WiedemannSolver::findNullspaceElement");

	return OK;
}

template <class Field, class Vector>
bool WiedemannSolver<Field, Vector>::certifyInconsistency (Vector                          &u,
							   const BlackboxArchetype&A,
							   const Vector                    &b)
{
	commentator.start ("Obtaining certificate of inconsistency (Wiedemann)",
			   "WiedemannSolver::certifyInconsistency");

	Vector PTinvu;
	typename Field::Element uTb;

	SolverTraits<WiedemannTraits> cert_traits;

	bool ret = false;

	cert_traits.preconditioner (WiedemannTraits::NONE);
	cert_traits.certificate (false);
	cert_traits.singular (WiedemannTraits::SINGULAR);
	cert_traits.maxTries (1);

	WiedemannSolver solver (_F, cert_traits, _randiter);

	Transpose<Vector> AT (&A);

	solver.findNullspaceElement (u, AT);
	_VD.dot (uTb, u, b);

	if (!_F.isZero (uTb))
		ret = true;

	commentator.stop (MSG_STATUS (ret), NULL, "WiedemannSolver::certifyInconsistency");

	return ret;
}

template <class Field, class Vector>
const BlackboxArchetype*WiedemannSolver<Field, Vector>::precondition
	(const BlackboxArchetype&A,
	 BlackboxArchetype*&PAQ,
	 BlackboxArchetype*&P,
	 BlackboxArchetype*&Q)
{
	switch (_traits.preconditioner ()) {
	    case WiedemannTraits::BUTTERFLY:
	    {
		    commentator.start ("Constructing butterfly preconditioner");

		    CekstvSwitchFactory<Field> factory (_randiter);
		    P = new Butterfly<Field, CekstvSwitch<Field> > (_F, A.rowdim (), factory);
		    Q = new Butterfly<Field, CekstvSwitch<Field> > (_F, A.coldim (), factory);
		    // WARNING: Memory leaks here!!!!!!
		    Compose<Vector> *AQ = new Compose<Vector> (&A, Q);
		    PAQ = new Compose<Vector> (P, AQ);

		    commentator.stop ("done");
		    break;
	    }

	    case WiedemannTraits::SPARSE:
	    {
		    commentator.start ("Constructing sparse preconditioner");

		    SparseMatrix<Field> *QT;
		    SparseMatrix<Field> *Q_sparse = new SparseMatrix<Field> (_F, A.coldim (), A.coldim ());
		    P = makeLambdaSparseMatrix (A.rowdim ());
		    QT = makeLambdaSparseMatrix (A.coldim ());

		    QT->transpose (*Q_sparse);
		    Q = Q_sparse;

		    // WARNING: Memory leaks here!!!!!!
		    Compose<Vector> *AQ = new Compose<Vector> (&A, Q);
		    PAQ = new Compose<Vector> (P, AQ);

		    commentator.stop ("done");
		    break;
	    }

	    case WiedemannTraits::TOEPLITZ:
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Toeplitz preconditioner not implemented yet. Sorry." << std::endl;

	    case WiedemannTraits::NONE:
		return &A;

	    default:
		throw PreconditionFailed (__FUNCTION__, __LINE__, "preconditioner is BUTTERFLY, SPARSE, or TOEPLITZ");
	}

	return PAQ;
}

template <class Field, class Vector>
SparseMatrix<Field, Vector> *WiedemannSolver<Field, Vector>::makeLambdaSparseMatrix (size_t m)
{
	const double             LAMBDA = 3;
	integer                  card;

	_F.cardinality (card);

	double                   init_p = 1.0 - 1.0 / (double) card;
	double                   log_m = LAMBDA * log ((double) m) / M_LN2;
	double                   new_p;

	SparseMatrix<Field>    *P = new SparseMatrix<Field> (_F, m, m);

	RandomSparseStream<Field> stream (_F, _randiter, init_p, m, m);

	for (unsigned int i = 0; i < m; ++i) {
		new_p = log_m / (m - i + 1);

		if (init_p < new_p)
			stream.setP (init_p);
		else
			stream.setP (new_p);

		stream >> P->getRow (i);
	}

	return P;
}

}

#endif // __WIEDEMANN_INL
