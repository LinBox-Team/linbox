/* -*- mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/* *******************************************************************
 *    ntl-hankel.h 
 * Copyright (C) 2003 Austin Lobo, B. David Saunders
 *    Template for Hankel specification for ntl Arithmetic
 *    Linbox version 2001 and 2002 from a version 
 *    Designed by A.Lobo and B.D. Saunders in 4/98
 *-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#ifndef NTL_HANKEL_H
#define NTL_HANKEL_H

#include <linbox/blackbox/blackbox-interface.h>
#include "ntl-toeplitz.h" // we inherit everything from ntl-toeplitz

//#define DBGMSGS 1

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   Partial Specialization of Hankel for Dense vectors from 
 *   an FFT based on Shoup's NTL library. Extends toeplitz matrix
 *-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

namespace LinBox
{
/// \ingroup blackbox
  template <class _Field>
    class Hankel: public BlackboxInterface, public Toeplitz<_Field>
    {
	protected: 
		using Toeplitz<_Field>:: shape;
		using Toeplitz<_Field>:: data;
		using Toeplitz<_Field>:: pdata;
		using Toeplitz<_Field>:: colDim;
		using Toeplitz<_Field>:: rowDim;
		using Toeplitz<_Field>:: sysDim;
		using Toeplitz<_Field>:: K;
		using Toeplitz<_Field>:: UnimodLT;
		using Toeplitz<_Field>:: UnimodUT;
    public:
		using Toeplitz<_Field>:: coldim;
		using Toeplitz<_Field>:: rowdim;
	typedef _Field Field;
      typedef typename Field::Element Element;
      
      //------- CONSTRUCTORS AND DESTRUCTORS
      
      ~Hankel();                // Destructor
      Hankel();                 // Zero Param Constructor
      Hankel( const Field F,    // Cnstr. with Field and STL vec. of elems
	      const std::vector<Element>&v);
      //	  Hankel(char *dataFileName ); // read from a file
      
      //------- INHERITED READ-ONLY ACCESSOR, and OBSERVER METHODS 
      
		void   print( std::ostream& os = std::cout) const; // Print to stdout
		void   print( char *outFileName) const;            // Print to file
       /*      inline size_t rowdim() const;// Number of Rows
		*      inline size_t coldim() const;// Number of Cols
		*      inline size_t sysdim() const;// Max of rows & columns; 
		*/

      //------- MUTATOR METHODS
      
      void setToUniModUT() ;      // Convert to UTriang matrix with det 1
      void setToUniModLT() ;      // Convert to LTriang matrix with det 1
      
      //------ SERVICE METHODS
		template<class OutVector, class InVector>
		OutVector& apply( OutVector &v_out, const InVector& v_in) const;

		template<class OutVector, class InVector>
		OutVector& applyTranspose( OutVector &v_out, const InVector& v_in) const;
      
    }; //  class Hankel
  
} // namespace Linbox

#include <linbox/blackbox/ntl-hankel.inl>     

#endif
