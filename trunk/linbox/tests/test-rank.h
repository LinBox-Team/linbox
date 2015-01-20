/* tests/test-rank.h
 * Time-stamp: <08 Aug 14 07:36:49 Jean-Guillaume.Dumas@imag.fr>
 * -----------------------------------------------------
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
 */


/*! @file  tests/test-rank.h
 * @ingroup tests
 * @brief  no doc
 * @test no doc.
 */



#include "linbox/linbox-config.h"

#define LINBOX_USE_BLACKBOX_THRESHOLD 100
#define LINBOX_COO_TRANSPOSE 100 /*  this is supposed to be triggerd half the time */
#define LINBOX_CSR_TRANSPOSE 100 /*  this is supposed to be triggerd half the time */
#define LINBOX_ELL_TRANSPOSE 100 /*  this is supposed to be triggerd half the time */
#define LINBOX_ELLR_TRANSPOSE 100 /*  this is supposed to be triggerd half the time */

#include <iostream>
#include <fstream>
#include <cstdio>
#include <givaro/modular.h>

#include "linbox/util/commentator.h"
#include "linbox/field/modular.h"
#include "linbox/field/PID-integer.h"
#include "linbox/field/gf2.h"
#include "linbox/blackbox/diagonal.h"
#include "linbox/matrix/sparse-matrix.h"
#include "linbox/blackbox/scalar-matrix.h"
#include "linbox/blackbox/direct-sum.h"
#include "linbox/algorithms/gauss.h"
#include "linbox/algorithms/gauss-gf2.h"
#include "linbox/solutions/rank.h"

#include "test-common.h"

using namespace LinBox;

// tests 1 and 2 were certain diagonals - now deemed unnecessary.  -bds 2005Mar15

/* Test 3: Rank of a random sparse matrix
 *
 * Constructs a random sparse matrix and computes its rank using Gaussian
 * elimination (direct and blas) and Wiedemann's algorithm. Checks that the results match.
 */
template <class BlackBox>
bool testRankMethods(const typename BlackBox::Field & F, size_t n, size_t m, unsigned int iterations, double sparsity = 0.05)
{
	typedef typename BlackBox::Field Field ;
	commentator().start ("Testing elimination-based and blackbox rank", "testRankMethods", (unsigned int)iterations);

	bool ret = true;
	unsigned int i;

	unsigned long rank_blackbox, rank_elimination, rank_hybrid;

	typename Field::RandIter ri (F);

	for (i = 0; i < iterations; ++i) {
		commentator().startIteration (i);

		RandomSparseStream<Field, typename BlackBox::Row> stream (F, ri, sparsity, n, m);
		BlackBox A (F, stream);
		// std::cout << A.rowdim() << ',' << A.coldim() << std::endl;

		F.write( commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION)) << endl;
		A.write( commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION),Tag::FileFormat::Maple ) << endl;

		Method::Blackbox MB;
		LinBox::rank (rank_blackbox, A, MB);
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "blackbox rank " << rank_blackbox << endl;

		Method::Elimination ME;
		LinBox::rank (rank_elimination, A, ME);
		if (rank_blackbox != rank_elimination) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: blackbox rank != elimination rank " << rank_elimination << endl;
			ret = false;
		}

		Method::Hybrid MH;
		LinBox::rank (rank_hybrid, A, MH);
		if (rank_blackbox != rank_hybrid) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: blackbox rank != hybrid rank " << rank_hybrid << endl;
			ret = false;
		}

		unsigned long rank_Wiedemann;
		//Method::Wiedemann MW;  // rank soln needs fixing for this.
		Method::Blackbox MW;
		LinBox::rank (rank_Wiedemann, A, MW);
		if (rank_Wiedemann != rank_blackbox ) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Ranks are not equal" << endl;
			ret = false;
		}

		unsigned long rank_blas_elimination ;
		if (F.characteristic() < LinBox::BlasBound && F.characteristic() == F.cardinality()) {
			Method::BlasElimination MBE;
			LinBox::rank (rank_blas_elimination, A, MBE);
		} else {
			rank_blas_elimination = rank_elimination;
		}

		commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION)
		<< "Rank computed by Wiedemann: " << rank_Wiedemann << endl
		<< "Rank computed by sparse elimination: " << rank_elimination << endl
		<< "Rank computed by blas_elimination: " << rank_blas_elimination << endl;

		if ( rank_blackbox != rank_blas_elimination) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
			<< "ERROR: Ranks are not equal" << endl;
			ret = false;
		}

		commentator().stop ("done");
		commentator().progress ();
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testEliminationRank");

	return ret;
}

// this test just doesn't work/compile
#if 0
bool testRankMethodsGF2(const GF2& F2, size_t n, unsigned int iterations, double sparsity = 0.05)
{
	typedef ZeroOne<GF2> Blackbox;
	typedef SparseMatrix<Givaro::Modular<double>,Vector<Givaro::Modular<double> >::SparseSeq> MdBlackbox;
	Givaro::Modular<double> MdF2(2);
	GF2::Element one; Givaro::Modular<double>::Element mdone;
	MdF2.assign(mdone,MdF2.one);


	commentator().start ("Testing elimination-based and blackbox rank over GF2", "testRankMethodsGF2", (unsigned int)iterations);

	bool ret = true;
	unsigned int i;

	unsigned long rank_blackbox, rank_elimination, rank_sparselimination, rank_sparse;
	//unsigned long rank_Wiedemann, rank_elimination, rank_blas_elimination;

	GF2::RandIter ri (F2);

	for (i = 0; i < iterations; ++i) {
		commentator().startIteration (i);

		Blackbox A(F2,n,n);
		MdBlackbox B(MdF2,n,n);
		for(size_t ii=0; ii<n;++ii) {
			for(size_t jj=0; jj<n; ++jj) {
				if (drand48()<sparsity) {
					A.setEntry(ii,jj,F2.one);
					B.setEntry(ii,jj,mdone);
				}
			}
		}

		F2.write( commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION)) << endl;
		B.write( commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION),Tag::FileFormat::Guillaume ) << endl;
		A.write( commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION),Tag::FileFormat::Guillaume ) << endl;


		LinBox::rank (rank_blackbox, A, Method::Blackbox ());
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "blackbox rank " << rank_blackbox << endl;

			LinBox::rank (rank_elimination, B, Method::BlasElimination());
		if (rank_blackbox != rank_elimination) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: blackbox rank != BLAS elimination rank " << rank_elimination << endl;
			ret = false;
		}

		rankin (rank_sparselimination, A, Method::SparseElimination());
		if (rank_blackbox != rank_sparselimination) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: blackbox rank != sparse elimination GF2 rank " << rank_elimination << endl;
			ret = false;
		}


		rankin (rank_sparse, B, Method::SparseElimination());

		if (rank_sparselimination != rank_sparse) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: rank sparse elimination GF2 != sparse rank " << rank_sparse << endl;
			ret = false;
		}

		commentator().stop ("done");
		commentator().progress ();
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testEliminationRank");

	return ret;
}
#endif

/* Test 4: Rank of zero and identity matrices by Wiedemann variants
 *
 */
template <class Field>
bool testZeroAndIdentRank (const Field &F, size_t n, unsigned int iterations)
{
	typedef ScalarMatrix<Field> Blackbox;

	commentator().start ("Testing rank of zero and Identity and half/half matrices", "testZeroAndIdentRank", (unsigned int)iterations);

	bool ret = true;
	unsigned int i;

	unsigned long r; // rank

	for (i = 0; i < iterations; ++i) {
		commentator().startIteration (i);


		Blackbox A (F, n, n, F.zero);
		Method::Wiedemann MW;
		LinBox::rank (r, A, MW);
		if (r != 0) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Wiedemann Rank of 0 is not 0, but is " << r << endl;
			ret = false;
		}

		Blackbox I (F, n, n, F.one);
		LinBox::rank (r, I, MW);
		if (r != n) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Wiedemann Rank of I is " << r << ", should be " << n << endl;
			ret = false;
		}

		DirectSum<Blackbox> B(A, I);
		LinBox::rank (r, B, MW);
		if (r != n) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Wiedemann Rank of I+0 is " << r << ", should be " << n << endl;
			ret = false;
		}

		Method::Wiedemann MWS(Method::Wiedemann::SYMMETRIC);
		LinBox::rank (r, B, MWS);
		if (r != n) {
			commentator().report (Commentator::LEVEL_IMPORTANT, INTERNAL_ERROR)
				<< "ERROR: Symmetric Wiedemann Rank of I+0 is " << r << ", should be " << n << endl;
			ret = false;
		}
		commentator().stop ("done");
		commentator().progress ();
	}

	commentator().stop (MSG_STATUS (ret), (const char *) 0, "testZeroAndIdentRank");

	return ret;
}

// Test the rank methods on each of several storage schemes for sparse matrices.
template <class Field>
bool testSparseRank(const Field &F, const size_t & n, size_t m, const size_t & iterations, const double & sparsity)
{
	bool pass = true ;
	F.write(commentator().report (Commentator::LEVEL_NORMAL, INTERNAL_DESCRIPTION)
	<< "over ") << endl;

	{
		typedef SparseMatrix<Field,SparseMatrixFormat::SparseSeq > Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::SparsePar > Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		// typedef SparseMatrix<Field,SparseMatrixFormat::SparseMap > Blackbox;
		// typedef Protected::SparseMatrixGeneric<Field,typename Vector<Field>::SparseMap > Blackbox;
		// if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::COO> Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::CSR> Blackbox; // inf loop
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::ELL> Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::ELL_R> Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
#if 0
	{
		TYPEdef SparseMatrix<Field,SparseMatrixFormat::HYB> Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
#endif
#if 0
	{
		typedef SparseMatrix<Field,SparseMatrixFormat::TPL> Blackbox;
		if (!testRankMethods<Blackbox> (F, n, m, (unsigned int)iterations, sparsity)) pass = false;
	}
#endif


	if (!testZeroAndIdentRank (F, n, 1)) pass = false;

	return pass ;


}

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
