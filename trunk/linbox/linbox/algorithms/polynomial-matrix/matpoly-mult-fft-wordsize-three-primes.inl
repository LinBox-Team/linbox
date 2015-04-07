/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2015  Pascal Giorgi
 *
 * Written by Pascal Giorgi   <pascal.giorgi@lirmm.fr>
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
#ifndef __LINBOX_matpoly_mult_ftt_wordsize_three_primes_INL
#define __LINBOX_matpoly_mult_ftt_wordsize_three_primes_INL

#include "givaro/modular.h"
#include "fflas-ffpack/fflas-ffpack.h"
#include "linbox/matrix/polynomial-matrix.h"
#include "linbox/matrix/matrix-domain.h"
#include "linbox/algorithms/polynomial-matrix/polynomial-fft-transform.h"
#include "linbox/algorithms/polynomial-matrix/matpoly-mult-fft-wordsize-fast.inl"
#include "linbox/randiter/random-fftprime.h"


namespace LinBox {

	/***********************************************************************************
	 **** Polynomial Matrix Multiplication over Zp[x] with p (FFLAS prime) ***
	 ***********************************************************************************/
	template<class Field>
	class PolynomialMatrixThreePrimesFFTMulDomain {
	public:
		// Polynomial matrix stored as a matrix of polynomial
		typedef PolynomialMatrix<PMType::polfirst,PMStorage::plain,Field> MatrixP;
		// Polynomial matrix stored as a polynomial of matrix
		typedef PolynomialMatrix<PMType::matfirst,PMStorage::plain,Field> PMatrix;
		//typedef Givaro::Modular<double>                ModField;
		typedef Field ModField;
	private:
		const Field              *_field;  // Read only
		uint64_t                      _p;
	  
	public:
		inline const Field & field() const { return *_field; }
	  
		PolynomialMatrixThreePrimesFFTMulDomain(const Field &F)
			: _field(&F), _p(field().cardinality())
		{
			if (integer(_p).bitsize()>29) {
				std::cout<<"MatPoly MUL FFT 3-primes: error initial prime has more than 29 bits exiting..."<<std::endl;
				throw LinboxError("LinBox ERROR: too large FFT Prime (more than 29 bits \n");
			}
		}

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
			MatrixP a2(field(),a.rowdim(),a.coldim(),pts);
			MatrixP b2(field(),b.rowdim(),b.coldim(),pts);
			a2.copy(a,0,a.size()-1);
			b2.copy(b,0,b.size()-1);
			// resize c to 2^lpts
			c.resize(pts);
			mul_fft (lpts,c, a2, b2);
			c.resize(deg);
		}

		void mul_fft (size_t lpts, MatrixP &c, MatrixP &a, MatrixP &b) {
			size_t pts=c.size();
			if ((_p-1) % pts == 0){
				PolynomialMatrixFFTPrimeMulDomain<ModField> fftprime_domain (field());
				fftprime_domain.mul_fft(lpts,c,a,b);
				return;
			}
			//std::cout<<"a:="<<a<<std::endl;
			//std::cout<<"b:="<<b<<std::endl;			
			linbox_check(a.coldim() == b.rowdim());
			size_t m = a.rowdim();
			size_t k = a.coldim();
			size_t n = b.coldim();
			
			size_t _k=k,lk=0;
			integer bound=integer(_p)*integer(_p)*integer(k)*pts;
			// compute bit size of feasible prime for FFLAS
			while ( _k ) {_k>>=1; ++lk;}
			size_t prime_bitsize= (53-lk)>>1;
			RandomFFTPrime RdFFT(prime_bitsize);
			std::vector<integer> bas;
			if (!RdFFT.generatePrimes(lpts,bound,bas)){
				std::cout<<"COULD NOT FIND ENOUGH FFT PRIME for in MatPoly 3-Primes FFT MUL exiting..."<<std::endl;
				throw LinboxError("LinBox ERROR: not enough FFT Prime\n");
			}
			size_t num_primes = bas.size();
			std::vector<double> basis(num_primes);
			std::copy(bas.begin(),bas.end(),basis.begin());
	    
			std::vector<MatrixP*> c_i (num_primes);
			std::vector<ModField> f(num_primes,ModField(2));
			for (size_t l=0;l<num_primes;l++)
				f[l]=ModField(basis[l]);
	    
			for (size_t l=0;l<num_primes;l++){
				PolynomialMatrixFFTPrimeMulDomain<ModField> fftdomain (f[l]);
				MatrixP ai(f[l],m,k,pts);
				MatrixP bi(f[l],k,n,pts);
				FFLAS::fassign(f[l],m*k*pts,a.getPointer(),1,ai.getWritePointer(),1);
				FFLAS::fassign(f[l],k*n*pts,b.getPointer(),1,bi.getWritePointer(),1);
				c_i[l] = new MatrixP(f[l], m, n, pts);
				fftdomain.mul_fft(lpts, *c_i[l], ai, bi);				
				//std::cout<<"pi:="<<(uint64_t)basis[l]<<std::endl;
				//std::cout<<"ci:="<<*c_i[l]<<std::endl;
			}
	    
			// reconstruct the result with MRS
			typename Field::Element alpha;
			typename Field::Element beta=field().one;
			FFLAS::freduce(field(),m*n*c.size(),c_i[0]->getPointer(),1,c.getWritePointer(),1);
			for (size_t i=1;i<num_primes;i++){
				for(size_t j=0;j<i;j++){
					f[i].init(alpha,basis[j]);
					f[i].invin(alpha);
					FFLAS::fsubin (f[i],m*n*pts,c_i[j]->getPointer(),1,c_i[i]->getWritePointer(),1);
					FFLAS::fscalin(f[i],m*n*pts,alpha,c_i[i]->getWritePointer(),1);
				}
				field().mulin(beta,basis[i-1]);
				FFLAS::faxpy(field(),m*n*pts,beta,c_i[i]->getPointer(),1,c.getWritePointer(),1);
			}

			//std::cout<<"c:="<<c<<std::endl;
			
			for (size_t i=1;i<num_primes;i++)
				delete c_i[i];
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
						std::swap(a2.ref(j,i),a2.ref(j,hdeg-1-i));
			else
				for (size_t j=0;j<b2.rowdim()*b2.coldim();j++)
					for (size_t i=0;i<hdeg/2;i++)
						std::swap(b2.ref(j,i),b2.ref(j,hdeg-1-i));

			midproduct_fft (lpts,c2, a2, b2, smallLeft);
			c.copy(c2,0,c.size()-1);
		}

		
		// a,b and c must have size: 2^lpts
		// -> a must have been already reversed according to the midproduct algorithm
		void midproduct_fft (size_t lpts, MatrixP &c, MatrixP &a, MatrixP &b,
				     bool smallLeft=true) {
			size_t pts=c.size();			
			if ((_p-1) % pts == 0){
				PolynomialMatrixFFTPrimeMulDomain<ModField> fftprime_domain (field());
				fftprime_domain.midproduct_fft(lpts,c,a,b,smallLeft);
				return;
			}
			size_t m = a.rowdim();
			size_t k = a.coldim();
			size_t n = b.coldim();
			size_t _k=k,lk=0;
			integer bound=integer(_p)*integer(_p)*integer(k)*pts;
			// compute bit size of feasible prime for FFLAS
			while ( _k ) {_k>>=1; ++lk;}
			size_t prime_bitsize= (53-lk)>>1;
			RandomFFTPrime RdFFT(prime_bitsize);
			std::vector<integer> bas;
			if (!RdFFT.generatePrimes(lpts,bound,bas)){
				std::cout<<"COULD NOT FIND ENOUGH FFT PRIME for in MatPoly 3-Primes FFT MUL exiting..."<<std::endl;
				throw LinboxError("LinBox ERROR: not enough FFT Prime\n");
			}
			size_t num_primes = bas.size();

			std::vector<double> basis(num_primes);
			std::copy(bas.begin(),bas.end(),basis.begin());
	    
			std::vector<MatrixP*> c_i (num_primes);
			std::vector<ModField> f(num_primes,ModField(2));
			for (size_t l=0;l<num_primes;l++)
				f[l]=ModField(basis[l]);
	    
			for (size_t l=0;l<num_primes;l++){
				PolynomialMatrixFFTPrimeMulDomain<ModField> fftdomain (f[l]);
				MatrixP ai(f[l],m,k,pts);
				MatrixP bi(f[l],k,n,pts);
				FFLAS::fassign(f[l],m*k*pts,a.getPointer(),1,ai.getWritePointer(),1);
				FFLAS::fassign(f[l],k*n*pts,b.getPointer(),1,bi.getWritePointer(),1);
				c_i[l] = new MatrixP(f[l], m, n, pts);
				fftdomain.midproduct_fft(lpts, *c_i[l], ai, bi,smallLeft);				
				//std::cout<<"pi:="<<(uint64_t)basis[l]<<std::endl;
				//std::cout<<"ci:="<<*c_i[l]<<std::endl;
			}
	    
			// reconstruct the result with MRS
			typename Field::Element alpha;
			typename Field::Element beta=field().one;
			FFLAS::freduce(field(),m*n*pts,c_i[0]->getPointer(),1,c.getWritePointer(),1);
			for (size_t i=1;i<num_primes;i++){
				for(size_t j=0;j<i;j++){
					f[i].init(alpha,basis[j]);
					f[i].invin(alpha);
					FFLAS::fsubin (f[i],m*n*pts,c_i[j]->getPointer(),1,c_i[i]->getWritePointer(),1);
					FFLAS::fscalin(f[i],m*n*pts,alpha,c_i[i]->getWritePointer(),1);
				}
				field().mulin(beta,basis[i-1]);
				FFLAS::faxpy(field(),m*n*pts,beta,c_i[i]->getPointer(),1,c.getWritePointer(),1);
			}

			//std::cout<<"c:="<<c<<std::endl;
			
			for (size_t i=1;i<num_primes;i++)
				delete c_i[i];
		
		}
		
	};
} // end of namespace LinBox

#endif
