// ======================================================================= //
// Copyright (C)  1999, Linbox project
// Givaro / Athapascan-1
// Valence computation
// Time-stamp: <04 Oct 01 18:30:44 Jean-Guillaume.Dumas@imag.fr> 
// ======================================================================= //
// Modified by Z. Wan to fit in linbox
#ifndef __LINBOX_VALENCE_C__
#define __LINBOX_VALENCE_C__

#include <math.h>
#include <vector>
#include <linbox/blackbox/compose.h>
#include <linbox/blackbox/sparse.h>
#include <linbox/blackbox/dense.h>
#include <linbox/blackbox/transpose.h>
#include <linbox/field/modular-int32.h>
#include <linbox/solutions/minpoly.h>
#include <linbox/randiter/random-prime.h>
#include <linbox/algorithms/matrix-mod.h>
#include <linbox/vector/sparse.h>
#include <linbox/vector/vector-traits.h>
#include <linbox/util/commentator.h>

namespace LinBox {
	

class Valence {
	public:

	// compute the bound for eigenvalue of AAT via oval of cassini
	// works with both SparseMatrix and DenseMatrix
	template <class Blackbox>
	static integer& cassini (integer& r, const Blackbox& A) {
		//commentator.start ("Cassini bound", "cassini");
    	integer _aat_diag, _aat_radius, _aat_radius1;
    	typedef typename Blackbox::Field Ring;
		_aat_diag = 0; _aat_radius = 0, _aat_radius1 = 0;

        std::vector< integer > d(A. rowdim()),w(A. coldim());
        std::vector<integer>::iterator di, wi;
        for(wi = w.begin();wi!= w.end();++wi) 
            *wi = 0;
        for(di = d.begin();di!= d.end();++di) 
            *di = 0;
		//typename Blackbox::ConstRowIterator row_p;
		typename Blackbox::Element tmp_e;
		Ring R(A. field());
		integer tmp; size_t i, j;
	
		for (j = 0, di = d. begin(); j < A. rowdim(); ++ j, ++ di) {
			// not efficient, but I am not tired of doing case by case
			for ( i = 0; i < A. coldim(); ++ i) {
				R. assign(tmp_e, A.getEntry( j, i));
				R. convert (tmp, tmp_e);
				if (tmp != 0) {
					*di += tmp * tmp;
					w [(int) i] += abs (tmp);
				}

            _aat_diag = _aat_diag >= *di ? _aat_diag : *di;
            }
        }

		for (j = 0, di = d. begin(); j < A. rowdim(); ++ j, ++ di) {
           	integer local_radius = 0;
			for (i = 0; i < A. coldim(); ++ i) {
				R. assign (tmp_e, A. getEntry (j, i));
				R. convert (tmp, tmp_e);
				if (tmp != 0) 
					local_radius += abs (tmp) * w[(int)i];
			}
			local_radius -= *di;
			if ( local_radius > _aat_radius1) {
				if ( local_radius > _aat_radius) {
					_aat_radius1 = _aat_radius;
					_aat_radius = local_radius;
				} else
					_aat_radius1 = local_radius;
			}
		}

        r = _aat_diag + (integer)sqrt( _aat_radius * _aat_radius1 );
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
			std::cout << "Cassini bound (AAT) =: " << r << std::endl;
		//commentator. stop ("done", NULL, "cassini");
		return r;
    }   

	// compute one valence of AAT over a field
	template <class Blackbox>
	static void one_valence(typename Blackbox::Element& v, unsigned long& r, const Blackbox& A) {
		//commentator.start ("One valence", "one valence");
		typedef std::vector<typename Blackbox::Element> Poly; Poly poly;
		typename Blackbox::Field F(A. field());
		Transpose<Blackbox> AT (&A);
		Compose<Blackbox, Transpose<Blackbox> > AAT(&A, &AT);
		minpoly (poly, AAT, A. field());
		typename Poly::iterator p;
		F. init (v, 0);

		for (p = poly. begin(); p != poly. end(); ++ p)
			if (! F. isZero (*p)) {
				F. assign (v, *p);
				break;
			}
		
		r = poly. size() -1;
		std::ostream& report = commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
			
		//	std::ostream& report = std::cout;
		report << "one valence =: " << v << " over ";
		A. field(). write(report); report << std::endl;
		//commentator. stop ("done", NULL, "one valence");
		return;
	}

	// compute the valence of AAT over an integer ring
	template <class Blackbox>
	static void valence(Integer& val, const Blackbox& A) {
		commentator. start ("Valence (AAT)", "Valence");
		typedef Modular<int32> Field;
		typedef typename Convert<Blackbox, Field>::value_type FBlackbox;
		FBlackbox* Ap;
		int n_bit = (int)(log((double)Field::getMaxModulus()) / M_LN2 - 2);
		unsigned long d; long m;
    	RandomPrime g(n_bit); Field::Element v;
		m = g. randomPrime ();
		Field F(m);
		MatrixMod::mod (Ap, A, F);
		one_valence(v, d, *Ap);
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
			std::cout<<"degree of minpoly of AAT: " << d << std::endl;
		delete Ap;
		valence (val, d, A);
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Integer valence =: " << val << std::endl;
		commentator. stop ("done", NULL, "Valence");
		return;
	}

	// compute the valence of AAT over an integer ring
	// d, the degree of min_poly of AAT
	template <class Blackbox>
	static void valence(Integer& val, unsigned long d, const Blackbox& A) {

		typedef Modular<int32> Field;
		typedef typename Convert<Blackbox, Field>::value_type FBlackbox;
		FBlackbox* Ap;
		int n_bit = (int)(log((double)Field::getMaxModulus()) / M_LN2 - 2);
    	RandomPrime rg(n_bit);
		std::vector<integer> Lv, Lm;
		unsigned long d1; long m; Field::Element v; integer im = 1;
		//compute an upper bound for val.
		integer bound; cassini (bound, A); bound = pow (bound, d); bound *= 2;
		commentator.report (Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION)
			<< "Bound for valence: " << bound << std::endl;

		do {
			m = rg. randomPrime ();
			Field F(m);
			MatrixMod::mod (Ap, A, F);
			one_valence(v, d1, *Ap);
			delete Ap;
			if (d1 == d) {
				im *= m;
				Lm. push_back (integer(m)); Lv. push_back (integer(v));
			}
		} while (im < bound);

		val = 0;
		std::vector<integer>::iterator Lv_p, Lm_p; integer tmp, a, b, g;
		for (Lv_p = Lv. begin(), Lm_p = Lm. begin(); Lv_p != Lv. end(); ++ Lv_p, ++ Lm_p) {
			tmp = im / *Lm_p;
			gcd (g, *Lm_p, tmp, a, b);
			val += *Lv_p * b * tmp;
			val %= im;
		}

		if (sign (val) < 0)
			val += im;
		tmp = val - im;
		if (abs(tmp) < abs(val)) 
			val = tmp;

		return;
	}
};
} //End of LinBox
#endif
