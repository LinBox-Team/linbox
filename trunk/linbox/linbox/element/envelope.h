/* -*- mode: C++; style: linux -*- */

/* linbox/element/envelope.h
 * Copyright (C) 1999-2001 William J Turner,
 *               2001 Bradford Hovinen
 *
 * Written by William J Turner <wjturner@math.ncsu.edu>,
 *            Bradford Hovinen <hovinen@cis.udel.edu>
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

#ifndef __ELEMENT_ENVELOPE_H
#define __ELEMENT_ENVELOPE_H

#include <iostream>

#include "linbox/element/abstract.h"

namespace LinBox 
{ 
	// Forward declarations
	template <class Field> class FieldEnvelope;
	template <class Field> class RandIterEnvelope;

	/** element envelope template.
	 * Encapsulated class of FieldEnvelope class.
	 * This element has no knowledge of the field to which it belongs, 
	 * so all operations and functions requiring knolwedge of the field,
	 * such as addition and other arithmetic operations, must be supplied
	 * by the field and not the element.
	 */
	template <class Field>
	class ElementEnvelope : public ElementAbstract
	{
	    public:

		/** Default Constructor.
		 */
		ElementEnvelope () {}

		/** Constructor from the Field element to be wrapped.
		 * @param elem Field element object to be wrapped.
		 */
		ElementEnvelope (const typename Field::Element &elem) : _elem (elem) {}

		/** Copy constructor.
		 * Constructs ElementEnvelope object by copying the element
		 * it wraps.
		 * This is required to allow element objects to be passed by value
		 * into functions.
		 * In this implementation, this means copying the element E._elem.
		 * @param  E FieldEnvelope object.
		 */
		ElementEnvelope (const ElementAbstract &E)
			: _elem (static_cast<const ElementEnvelope&>(E)._elem) {}
  
		/** Virtual copy constructor.
		 * Required because constructors cannot be virtual.
		 * Passes construction on to derived classes.
		 * @return pointer to new element object in dynamic memory.
		 */
		ElementAbstract* clone (void) const { return new ElementEnvelope (*this); }

		/** Assignment operator.
		 * @return reference to self
		 * @param  x parameterized field base element
		 */
		ElementAbstract &operator= (const ElementAbstract &E)
		{
			if (this != &E) // guard against self-assignment
				_elem = static_cast<const ElementEnvelope&>(E)._elem;
			return *this;
		}

		/** Destructor.
		 */
		~ElementEnvelope () {}

	    private:

		// Friend declarations
		friend class FieldEnvelope<Field>;
		friend class RandIterEnvelope<Field>;

		typename Field::Element _elem;

	}; // class ElementEnvelope

} // namespace LinBox

#endif // __ELEMENT_ENVELOPE_H
