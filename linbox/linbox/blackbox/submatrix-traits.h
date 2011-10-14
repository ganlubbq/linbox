/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
/* Copyright (C) 2010 LinBox
 * Written by Zhendong Wan
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
#ifndef __LINBOX_submatrix_traits_H
#define __LINBOX_submatrix_traits_H

#include "linbox/blackbox/submatrix.h"


namespace LinBox
{

	template<class Matrix>
	class SubMatrixTraits;

	template<class Field>
	class SubMatrixTraits<BlasMatrix<Field> > {

	public:

		typedef  Submatrix<BlasMatrix<Field> > value_type;
	};

	template<class Field>
	class SubMatrixTraits<Submatrix<BlasMatrix<Field> > > {

	public:

		typedef Submatrix<BlasMatrix<Field> > value_type;
	};


}

#endif //__LINBOX_submatrix_traits_H

