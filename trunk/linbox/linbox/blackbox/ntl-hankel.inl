/* C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ 
 *    ntl-hankel.inl     NTL_Hankel.cpp file 
 *    Copyright (C) 2003 Austin Lobo, B. David Saunders
 *
 *    Author: Austin Lobo 
 *    Linbox version 2001 and 2002 
 *
 *    This file is included in the template description of ntl-Hankel.h
 *    it contains the implementations of templatized member functions in the 
 *    partial template  specialization for hankel matrices that
 *    are manipulated in fields and rings according to the arithmetic
 *    in the ntl package from V. Shoup
 *
 *    Everything is in the Linbox namespace by virtue of the #include
 *    in ntl-Hankel.h
 *-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#include <iostream>
#include <fstream>
#include <NTL/ZZ_pX.h>

namespace LinBox 
{
	/*-----------------------------------------------------------------
	 *----    Destructor
	 *----------------------------------------------------------------*/
	template <class Field>
	inline Hankel<Field>::~Hankel()
	{
#ifdef DBGMSGS
		std::cout << "Hankel::~Hankel():\tDestroyed a " << rowDim << "x"<< colDim<<
			" Hankel matrix "<< std::endl;
#endif
	}//---- Destructor ---- 
	
	
	
	/*-----------------------------------------------------------------
	 *----    Default Constructor    
	 *----------------------------------------------------------------*/
	template <class Field>
	Hankel<Field>::Hankel() 
	{
		shape  = HANKEL;
#ifdef DBGMSGS
		std::cout << "Hankel::Hankel():\tCreated a " << rowDim << "x"<< colDim<<
			" Hankel matrix "<< std::endl;
#endif
		
	}//----- Zero Param Constructor ---- [Tested 6/14/02 -- Works]
	
	
	
	
	/*-----------------------------------------------------------------
	 *----- Constructor With User-Supplied First Row And Column
	 *----------------------------------------------------------------*/
	template <class Field>
	Hankel<Field>::Hankel( const Field F,
					   const std::vector<typename Field::Element>&v) 
	{
		// Assumes that the input is a vector of ZZ_p else things will FAIL
		if ( (1 & v.size()) == 0) 
		{
			std::cout << "There must be an ODD number of entries in the input vector " <<
				"The length given is " << v.size();
		}
		assert( (1 & v.size()) == 1);
		
		rowDim = (1+v.size())/2; // The vector is 0..2n-2;
		colDim = (1+v.size())/2;
		sysDim = (1+v.size())/2;
		
		data = v;
		pdata.SetMaxLength( v.size());
		//		rpdata.SetMaxLength( v.size());
		for (unsigned int i=0; i< v.size(); i++) 
		{
			SetCoeff( pdata, i, v[i]);
			//SetCoeff( rpdata, i, v[v.size()-1-i]);
		}
		
#ifdef DBGMSGS
		std::cout << "Hankel::Hankel(F,V):\tCreated a " << rowDim << "x"<< colDim<<
			" Hankel matrix "<< std::endl;
#endif
		
	}//----- Constructor given a vector---- 
	
	
	
	/*-----------------------------------------------------------------
	 *-----    Print The Matrix To Screen
	 *----------------------------------------------------------------*/
	template <class Field>
	void Hankel<Field>::print(std::ostream& os) const 
	{
		register size_t i, N, j;
		
		os<< rowDim << " " << colDim << " " << shape << std::endl;
		N = data.size() - 1;
		
		if ( N < 20 ) {            // Print small matrices in dense format
			for (i = N ; i >= colDim-1; i--) {
				for ( j = 0; j < colDim ; j++)
					os << " " << data[i-j] ;
				os << std::endl;
			}
		} 
		else {
			// Print large matrices' first row and col
			os << rowDim << " " << colDim << " " << shape << std::endl ;
			os << "[";
			for (int i=data.size()-1; i>= 0;i--)
				os << data[i] << " ";
			os << "]\n";
			os << pdata << std::endl;
		} //[v(2n-2),....,v(0)]; where v(0) is the top right entry of the matrix
		
		return;
	} //---- print()----- [Tested 6/14/02 -- Works]
	
	
	
	
	/*-----------------------------------------------------------------
	 *----    The infamous clone has been created here 
	 *----------------------------------------------------------------*/
	//template <class Field, class Vector>
	//BlackboxArchetype<Vector>* Hankel<Field, Vector>::clone() const 
	//{ 
		//return new Hankel(*this); 
	//}// ------ This is not tested. 
	
	
	/*-----------------------------------------------------------------
	 *----    Save To File, Given Destination Filename
	 *----------------------------------------------------------------*/
	template <class Field>
	void Hankel<Field>::print( char *outFileName) const
	{
		int i, j, N;
		
		std::cout << "Printing hankel matrix to " << outFileName << std::endl;
		
		if ( outFileName == NULL ) 
			print();    // Print to stdout if no file is specified
		else { 
			std::ofstream o_fp(outFileName, std::ios::out);
			o_fp << rowDim << " " << colDim << " " << shape << std::endl ;
			o_fp << "[";
			for (i=data.size()-1; i>= 0;i--) o_fp << data[i] << " ";
			o_fp << "]\n";
			
			o_fp.close();
		}
		return;
	} // print(char *) 
	
	
	
	/*-----------------------------------------------------------------
	 *    Make the matrix LOWER triangular with determinant 1.
	 *    i.e. clear the last coldim-1 elements in the data vector
	 *----------------------------------------------------------------*/
	template <class Field>
	void Hankel<Field>::setToUniModLT()
	{
		int L = data.size()-1;
		shape = UnimodLT;

		long zero = 0;  // needed for NTL initialization of a polynomial coeff
		for (int i=rowDim-1; i <= L; i++ ) {
			K.init(data[i],0);     // zero out the below-diagonal entries 
			SetCoeff(pdata,i,zero);
		}
		K.init(data[rowDim-1],1);          // set the antidiagonal to 1
		SetCoeff( pdata, rowDim-1);       // update the corresponding coeff of pdata
		//reverse(rpdata,pdata);        // no need to construct the transpose
		return;
	}// 
	
	
	
	/*-----------------------------------------------------------------
	 *    Make matrix a unimodular UPPER Triangular with det 1
	 *    i.e. clear the first N-1 elements in the data vector
	 *    and make the elements below the anti-diagonal all zero
	 *----------------------------------------------------------------*/
	template <class Field>
	void Hankel<Field>::setToUniModUT()
	{
		shape = UnimodUT;
		
		long zero = 0;  // needed for NTL initialization of a polynomial coeff

		for (size_t i=0; i < rowDim-1; i++ ) {
			K.init(data[i],0);     // zero out the below-antidiagonal entries 
			SetCoeff(pdata, i , zero);
		}

		K.init(data[rowDim-1],1);      // set antidiagonal to 1
		SetCoeff(pdata,rowDim-1);      // update the corresponding coeff of pdata
		//reverse(rpdata,pdata);    // no need to construct the transpose
		
		return;
	}// 
	
	
	
	/*-----------------------------------------------------------------
	 *    Apply the matrix to a vector Here the input and output 
	 *    vectors are both over the SAME prime ZZ_p field as the 
	 *    Hankel matrix itself.
	 *----------------------------------------------------------------*/
	template <class Field>
	template<class OutVector, class InVector>
	OutVector& Hankel<Field>::apply( OutVector &v_out, 
										  const InVector& v_in) const
	{  
		if (v_out.size() != rowdim())
			std::cout << "\tToeplitz::apply()\t output vector not correct size, at "
					  << v_out.size() << ". System rowdim is" <<  rowdim() << std::endl;
		if ( v_out.size() != v_in.size())
			std::cout << "\tToeplitz::apply()\t input vector not correct size at " 
					  << v_in.size() << ". System coldim is" <<  coldim() << std::endl;
		assert((v_out.size() == rowdim()) && 
			   (v_in.size() == coldim()))  ;
		
		NTL::ZZ_pX pxOut, pxIn;
		pxIn.SetMaxLength( v_in.size()-1);
		for (unsigned int i=0; i< v_in.size(); i++)
			SetCoeff( pxIn, i, v_in[i]);
		
#ifdef DBGMSGS
		std::cout << "\npX in is " << pxIn << std::endl;
		std::cout << "multiplied by " << pdata << std::endl;
#endif
		mul(pxOut,pxIn,pdata);
		
#ifdef DBGMSGS
		std::cout <<"pxOut is " << pxOut << std::endl;
#endif
		int N = rowdim();
		for ( int i= 0; i < N; i++) 
			GetCoeff(v_out[N-1-i], pxOut, N-1+i);
		
		return v_out;
		
	}
	
	
	/*-----------------------------------------------------------------
	 *    Apply the transposed matrix to a vector Here the input and output 
	 *    vectors are both over the SAME prime ZZ_p field as the 
	 *    Hankel matrix itself. Calls the multiply from the Toeplitz matrix
	 *    Since Hankel is symmetric, this is the same as apply
	 *----------------------------------------------------------------*/
	template <class Field>
	template <class OutVector, class InVector>
	OutVector& Hankel<Field>::applyTranspose( OutVector &v_out, 
													 const InVector& v_in) const
	{  
		return(v_out = apply(v_out,v_in));

	}
	
	
	
} // namespace LinBox
