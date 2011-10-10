/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* Copyright (C) 2010 LinBox
 * Written by <brice.boyer@imag.fr>
 *
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*! @file algorithms/linbox-tags.h
 * @ingroup algorithms
 * @brief Provides tags for various algorithms/solutions, à la \c FFLAS.
 * This is a subset of Fflas* enums.
 */

#ifndef __LINBOX_linbox_tags_H
#define __LINBOX_linbox_tags_H

#include <fflas-ffpack/fflas/fflas.h>

namespace LinBox
{

	/*! Structure for tags.
	 * Tags are simple enums that set a choice in a routine.
	 * For instance, if the user wants a <i>right</i> nullspace,
	 * she will use a \c LinBoxTag::Right parameter.
	 *
	 * There it total compatiblity with \c FFLAS tags (cross link)
	 * For instance, in LinBox, it is similar to use \c LinBoxTag::Upper and
	 * <code>(LinBoxTag::Shape) FFLAS::FflasUpper</code>
	 *
	 * @note Tags are not Methods.
	 */
	struct LinBoxTag {
		//! Left/Right Tag
		enum Side {
			Left  = FFLAS::FflasLeft, //!< Left
			Right = FFLAS::FflasRight  //!< Right
		};

		enum Transpose {
			NoTrans = FFLAS::FflasNoTrans,
			Trans   = FFLAS::FflasTrans
		};

		enum Shape {
			Upper = FFLAS::FflasUpper,
			Lower = FFLAS::FflasLower
		} ;

		enum Diag {
			NonUnit = FFLAS::FflasNonUnit,
			Unit    = FFLAS::FflasUnit
		} ;
	} ;

}

#endif // __LINBOX_linbox_tags_H
