/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/util/debug.h
 * Copyright (C) 2001 Bradford Hovinen
 *
 * Written by Bradford Hovinen <hovinen@cis.udel.edu>
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

#ifndef __UTIL_DEBUG_H
#define __UTIL_DEBUG_H

#include <iostream>

#ifdef NDEBUG
#  define linbox_check(check)
#else
#  ifdef __GNUC__
#    define linbox_check(check) \
        if (!(check)) \
                 throw LinBox::PreconditionFailed (__FUNCTION__, __LINE__, #check);
#  else
#    define linbox_check(check) \
        if (!(check)) \
                 throw LinBox::PreconditionFailed (__FILE__, __LINE__, #check);
#  endif
#endif

namespace LinBox
{
	class PreconditionFailed
	{
		static std::ostream *_errorStream;

	    public:
		PreconditionFailed (const char *function, int line, const char *check) {
			if (_errorStream == (std::ostream *) 0)
				_errorStream = &std::cerr;

			(*_errorStream) << std::endl << std::endl;
			(*_errorStream) << "ERROR (" << function << ":" << line << "): ";
			(*_errorStream) << "Precondition " << check << " not met" << std::endl;
		}

		static void setErrorStream (std::ostream &stream);
	};
}

#ifdef LinBoxSrcOnly
// for all-source compilation
#include <linbox/util/debug.C>
#endif

#endif // __UTIL_DEBUG_H
