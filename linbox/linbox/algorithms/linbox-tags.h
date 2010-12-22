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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __LINBOX_linbox_tags_H
#define __LINBOX_linbox_tags_H

namespace LinBox
{

	/*! Structure for tags.
	 * Tags are simple enums that set a choice in a routine.
	 * For instance, if the user wants a <i>right</i> nullspace,
	 * she will use a \c LinBoxTag::Right parameter.
	 * @note Tags are not Methods.
	 */
	struct LinBoxTag {
		//! Left/Right Tag
		enum Side {
			Left  = 1000, //!< Left
			Right = 1001  //!< Right
		};

		enum Transpose {
			Trans   = 1010,
			NoTrans = 1011
		};
	} ;

}

#endif // __LINBOX_linbox_tags_H
