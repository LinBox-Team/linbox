/* -*- mode: c; style: linux -*- */

/* fraction-block.h
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * Written by Bradford Hovinen <hovinen@helixcode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef __FRACTION_BLOCK_H
#define __FRACTION_BLOCK_H

#include <gnome.h>

#include "block.h"

BEGIN_GNOME_DECLS

#define FRACTION_BLOCK(obj)          GTK_CHECK_CAST (obj, fraction_block_get_type (), FractionBlock)
#define FRACTION_BLOCK_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, fraction_block_get_type (), FractionBlockClass)
#define IS_FRACTION_BLOCK(obj)       GTK_CHECK_TYPE (obj, fraction_block_get_type ())

typedef struct _FractionBlock FractionBlock;
typedef struct _FractionBlockClass FractionBlockClass;
typedef struct _FractionBlockPrivate FractionBlockPrivate;

struct _FractionBlock 
{
	Block parent;

	FractionBlockPrivate *p;
};

struct _FractionBlockClass 
{
	BlockClass block_class;
};

guint fraction_block_get_type              (void);

/**
 * fraction_block_new:
 * @numerator: 
 * @denominator: 
 * 
 * Factory method
 * 
 * Return value: New fraction block object
 **/

GtkObject *fraction_block_new              (MathObject *numerator, 
					    MathObject *denominator);

/**
 * fraction_block_get_numerator:
 * @block: 
 * 
 * Get the numerator for the fraction
 * 
 * Return value: 
 **/

MathObject *fraction_block_get_numerator   (FractionBlock *block);

/**
 * fraction_block_get_denominator:
 * @block: 
 * 
 * Get the denominator for the fraction
 * 
 * Return value: 
 **/

MathObject *fraction_block_get_denominator (FractionBlock *block);

/**
 * fraction_block_set_numerator:
 * @block: 
 * @numerator: 
 * 
 * Set the numerator for the fraction; wrapper for gtk_object_set
 **/

void        fraction_block_set_numerator   (FractionBlock *block,
					    MathObject *numerator);

/**
 * fraction_block_set_denominator:
 * @block: 
 * @denominator: 
 * 
 * Set the denominator for the fraction; wrapper for gtk_object_set
 **/

void        fraction_block_set_denominator (FractionBlock *block,
					    MathObject *denominator);

END_GNOME_DECLS

#endif /* __FRACTION_BLOCK_H */
