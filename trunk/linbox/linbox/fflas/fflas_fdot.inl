/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/fflas/fflas_fger.inl
 * Copyright (C) 2005 Clement Pernet
 *
 * Written by Clement Pernet <Clement.Pernet@imag.fr>
 *
 * See COPYING for license information.
 */

// Default implementation
// Specializations using FieldAxpy should be written
// to increase efficiency
template<class Field>
inline typename Field::Element
LinBox::FFLAS::fdot( const Field& F, const size_t N, 
		     const typename Field::Element * x, const size_t incx,
		     const typename Field::Element * y, const size_t incy ){
	
	typename Field::Element d;
	const typename Field::Element* xi = x;
	const typename Field::Element* yi = y;
	F.init( d, 0 );
	for ( ; xi < x+N*incx; xi+=incx, yi+=incy )
		F.axpyin( d, *xi, *yi );
	return d;
}

template<>
inline LinBox::FFLAS::DoubleDomain::Element
LinBox::FFLAS::fdot( const DoubleDomain& F, const size_t N, 
		     const DoubleDomain::Element * x, const size_t incx,
		     const DoubleDomain::Element * y, const size_t incy ){
	
	return cblas_ddot( N, x, incx, y, incy );
}
