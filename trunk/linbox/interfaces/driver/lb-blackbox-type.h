/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* lb-blackbox-type.h
 * Copyright (C) 2005 Pascal Giorgi
 *
 * Written by Pascal Giorgi <pgiorgi@uwaterloo.ca>
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __LINBOX_LB_BLACKBOX_TYPE_H
#define __LINBOX_LB_BLACKBOX_TYPE_H


/****************************************
 * Define the list of all Blackbox Type *
 ****************************************/

// (NEED TO USE ENVELOPE TO DEFINE A CONCRETE TYPE)
typedef LinBoxTypelist < BlackboxEnvelope< LinBox::SparseMatrix > , LinBoxDumbType> BL1;
typedef LinBoxTypelist < BlackboxEnvelope< LinBox::BlasBlackbox > , BL1> BL2; 

// define the blackbox typelist
typedef BL2 BlackboxList;



/*********************************************
 * Update the Factory with all blackbox type *
 *********************************************/
extern Blackbox_Factory linbox_blackbox;

void UpdateBlackbox() {
	linbox_blackbox.add("linbox_sparse",
			    Blackbox_Factory::CallBackMap::value_type::second_type( constructBlackbox_from_size<LinBox::SparseMatrix>, 
										    constructBlackbox_from_stream<LinBox::SparseMatrix> ));
	linbox_blackbox.add("linbox_dense",
			    Blackbox_Factory::CallBackMap::value_type::second_type( constructBlackbox_from_size<LinBox::BlasBlackbox>, 
										    constructBlackbox_from_stream<LinBox::BlasBlackbox> ));			    
}


/*****************************
 * Default type for blackbox *
 *****************************/

// definition of the default type blackbox
#define default_blackbox  "linbox_dense"

#endif
