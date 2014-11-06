/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2013  Pascal Giorgi
 *                     Romain Lebreton
 *
 * Written by Pascal Giorgi   <pascal.giorgi@lirmm.fr>
 *            Romain Lebreton <lebreton@lirmm.fr>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */
#ifndef __LINBOX_matpoly_mult_ftt_wordsize_fast_INL
#define __LINBOX_matpoly_mult_ftt_wordsize_fast_INL

#include "linbox/field/modular.h"
#include "linbox/matrix/polynomial-matrix.h"
#include "linbox/matrix/matrix-domain.h"
#include "linbox/algorithms/polynomial-matrix/polynomial-fft-transform.h"

namespace LinBox {

	/***********************************************************************************
	 **** Polynomial Matrix Multiplication over Zp[x] with p (FFTPrime, FFLAS prime) ***
	 ***********************************************************************************/

	class PolynomialMatrixFFTPrimeMulDomain {

		typedef Modular<int32_t>     Field;
		typedef Field::Element     Element;

		public:
		// Polynomial matrix stored as a matrix of polynomial
		typedef PolynomialMatrix<PMType::polfirst,PMStorage::plain,Field> MatrixP;
		// Polynomial matrix stored as a polynomial of matrix
		typedef PolynomialMatrix<PMType::matfirst,PMStorage::plain,Field> PMatrix;

		private:
		const Field            *_field;  // Read only
		uint32_t                    _p;
		BlasMatrixDomain<Field>   _BMD;

		public:
		inline const Field & field() const { return *_field; }

		template< typename Field2>
			PolynomialMatrixFFTPrimeMulDomain(const Field2 &F) : _field(&F), _p(field().cardinality()), _BMD(F){}

		template<typename Matrix1, typename Matrix2, typename Matrix3>
			void mul (Matrix1 &c, const Matrix2 &a, const Matrix3 &b) {
				linbox_check(a.coldim()==b.rowdim());
				size_t deg  = a.size()+b.size()-1;
				size_t lpts = 0;
				size_t pts  = 1; while (pts < deg) { pts= pts<<1; ++lpts; }
				// padd the input a and b to 2^lpts (convert to MatrixP representation)
				MatrixP a2(field(),a.rowdim(),a.coldim(),pts);
				MatrixP b2(field(),b.rowdim(),b.coldim(),pts);
				a2.copy(a,0,a.size()-1);
				b2.copy(b,0,b.size()-1);
				MatrixP c2(field(),c.rowdim(),c.coldim(),pts);
				mul_fft (lpts,c2, a2, b2);
				c.copy(c2,0,deg-1);
			}

		void mul (MatrixP &c, const MatrixP &a, const MatrixP &b) {
			linbox_check(a.coldim()==b.rowdim());
			size_t deg  = a.size()+b.size()-1;
			size_t lpts = 0;
			size_t pts  = 1; while (pts < deg) { pts= pts<<1; ++lpts; }
			// padd the input a and b to 2^lpts
			MatrixP a2(a.field(),a.rowdim(),a.coldim(),pts);
			MatrixP b2(b.field(),b.rowdim(),b.coldim(),pts);
			a2.copy(a,0,a.size()-1);
			b2.copy(b,0,b.size()-1);
			// resize c to 2^lpts
			c.resize(pts);
			mul_fft (lpts,c, a2, b2);
			c.resize(deg);
		}

		// a,b and c must have size: 2^lpts
		// -> use TFT to circumvent the padding issue
		void mul_fft (size_t lpts, MatrixP &c, MatrixP &a, MatrixP &b) {
			FFT_PROFILE_START;
			size_t m = a.rowdim();
			size_t k = a.coldim();
			size_t n = b.coldim();
			size_t pts=c.size();
			//cout<<"mul : "<<pts<<endl;
#ifdef FFT_PROFILER
			if (FFT_PROF_LEVEL==1) std::cout<<"FFT: points "<<pts<<"\n";
#endif

			if ((_p-1) % pts != 0) {
				std::cout<<"Error the prime is not a FFTPrime or it has too small power of 2\n";
				throw LinboxError("LinBox ERROR: bad FFT Prime\n");
			}
			FFT_transform<Field> FFTer (field(), lpts);
			Element _inv_w = FFTer._w;
			field().invin(_inv_w);
			FFT_transform<Field> FFTinv (field(), lpts, _inv_w);
			FFT_PROFILING(1,"init");

			// FFT transformation on the input matrices
			for (size_t i = 0; i < m * k; i++)
				FFTer.FFT_DIF_Harvey_SSE(a(i));
			for (size_t i = 0; i < k * n; i++)
				FFTer.FFT_DIF_Harvey_SSE(b(i));
			FFT_PROFILING(1,"direct FFT_DIF");

			// convert the matrix representation to matfirst
			PMatrix vm_c (field(), m, n, pts);
			PMatrix vm_a (field(), m, k, pts);
			PMatrix vm_b (field(), k, n, pts);
			FFT_PROFILING(1,"creation of Matfirst");

			vm_a.copy(a);
			vm_b.copy(b);
			FFT_PROFILING(1,"Polfirst to Matfirst");

			// Pointwise multiplication
			for (size_t i = 0; i < pts; ++i)
				_BMD.mul(vm_c[i], vm_a[i], vm_b[i]);
			FFT_PROFILING(1,"Pointwise mult");

			// Transformation into matrix of polynomials
			c.copy(vm_c);
			FFT_PROFILING(1,"Matfirst to Polfirst");

			// Inverse FFT on the output matrix
			for (size_t i = 0; i < m * n; i++)
				FFTinv.FFT_DIT_Harvey_SSE(c(i));
			FFT_PROFILING(1,"inverse FFT_DIT");

			// Divide by pts = 2^lpts
			Element inv_pts;
			field().init(inv_pts, pts);
			field().invin(inv_pts);
			for (size_t i = 0; i < m * n; i++)
				for (size_t j = 0; j < pts; j++)
					field().mulin(c.ref(i,j), inv_pts);
			FFT_PROFILING(1,"scaling the result");
		}

		// compute  c= (a*b x^(-n0-1)) mod x^n1
		// by defaut: n0=c.size() and n1=2*c.size();
		template<typename Matrix1, typename Matrix2, typename Matrix3>
			void midproduct (Matrix1 &c, const Matrix2 &a, const Matrix3 &b,
					bool smallLeft=true, size_t n0=0,size_t n1=0) {
				linbox_check(a.coldim()==b.rowdim());
				size_t hdeg = (n0==0?c.size():n0);
				size_t deg  = (n1==0?2*hdeg:n1);
				linbox_check(c.size()>=deg-hdeg);
				if (smallLeft){
					linbox_check(b.size()<hdeg+deg);
				}
				else
					linbox_check(a.size()<hdeg+deg);

				size_t lpts = 0;
				size_t pts  = 1; while (pts < deg) { pts= pts<<1; ++lpts; }
				// padd the input a and b to 2^lpts (use MatrixP representation)
				MatrixP a2(field(),a.rowdim(),a.coldim(),pts);
				MatrixP b2(field(),b.rowdim(),b.coldim(),pts);
				MatrixP c2(field(),c.rowdim(),c.coldim(),pts);
				a2.copy(a,0,a.size()-1);
				b2.copy(b,0,b.size()-1);

				// reverse the element of the smallest polynomial according to h(x^-1)*x^(hdeg)
				if (smallLeft)
					for (size_t j=0;j<a2.rowdim()*a2.coldim();j++)
						for (size_t i=0;i<hdeg/2;i++)
							swap(a2.ref(j,i),a2.ref(j,hdeg-1-i));
				else
					for (size_t j=0;j<b2.rowdim()*b2.coldim();j++)
						for (size_t i=0;i<hdeg/2;i++)
							swap(b2.ref(j,i),b2.ref(j,hdeg-1-i));

				midproduct_fft (lpts,c2, a2, b2, smallLeft);
				c.copy(c2,0,c.size()-1);
			}

		// a,b and c must have size: 2^lpts
		// -> a must have been already reversed according to the midproduct algorithm
		void midproduct_fft (size_t lpts, MatrixP &c, MatrixP &a, MatrixP &b,
				bool smallLeft=true) {
			FFT_PROFILE_START;
			size_t m = a.rowdim();
			size_t k = a.coldim();
			size_t n = b.coldim();
			size_t pts=c.size();
			//cout<<"mid : "<<pts<<endl;
#ifdef FFT_PROFILER
			if (FFT_PROF_LEVEL==1) std::cout<<"FFT: points "<<pts<<"\n";
#endif
			if ((_p-1) % pts != 0) {
				std::cout<<"Error the prime is not a FFTPrime or it has too small power of 2\n";
				throw LinboxError("LinBox ERROR: bad FFT Prime\n");
			}
			FFT_transform<Field> FFTer (field(), lpts);
			Element _inv_w = FFTer._w;
			field().invin(_inv_w);
			FFT_transform<Field> FFTinv (field(), lpts, _inv_w);
			FFT_PROFILING(1,"init");

			// FFT transformation on the input matrices
			if (smallLeft){
				for (size_t i = 0; i < m * k; i++)
					FFTer.FFT_DIF_Harvey_SSE(a(i));
				for (size_t i = 0; i < k * n; i++)
					FFTinv.FFT_DIF_Harvey_SSE(b(i));
			}
			else {
				for (size_t i = 0; i < m * k; i++)
					FFTinv.FFT_DIF_Harvey_SSE(a(i));
				for (size_t i = 0; i < k * n; i++)
					FFTer.FFT_DIF_Harvey_SSE(b(i));
			}
			FFT_PROFILING(1,"direct FFT_DIF");

			// convert the matrix representation to matfirst
			PMatrix vm_c (field(), m, n, pts);
			PMatrix vm_a (field(), m, k, pts);
			PMatrix vm_b (field(), k, n, pts);
			FFT_PROFILING(1,"creation of Matfirst");

			vm_a.copy(a);
			vm_b.copy(b);
			FFT_PROFILING(1,"Polfirst to Matfirst");

			// Pointwise multiplication
			for (size_t i = 0; i < pts; ++i)
				_BMD.mul(vm_c[i], vm_a[i], vm_b[i]);
			FFT_PROFILING(1,"pointwise mult");

			// Transformation into matrix of polynomials
			c.copy(vm_c);
			FFT_PROFILING(1,"Matfirst to Polfirst");

			// Inverse FFT on the output matrix
			for (size_t i = 0; i < m * n; i++)
				FFTer.FFT_DIT_Harvey_SSE(c(i));
			FFT_PROFILING(1,"inverse FFT_DIT");

			// Divide by pts = 2^ltps
			Element inv_pts;
			field().init(inv_pts, pts);
			field().invin(inv_pts);
			for (size_t i = 0; i < m * n; i++)
				for (size_t j = 0; j < pts; j++)
					field().mulin(c.ref(i,j), inv_pts);
			FFT_PROFILING(1,"scaling the result");
		}
	}; // end of class special FFT mul domain



}//end of namespace LinBox

#endif // __LINBOX_matpoly_mult_ftt_wordsize_fast_INL
