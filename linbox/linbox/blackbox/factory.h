/* -*- mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* linbox/blackbox/factory.h
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <bghovine@math.uwaterloo.ca>
 *
 * ------------------------------------
 *
 * See COPYING for license information
 */

#ifndef __BLACKBOX_FACTORY_H
#define __BLACKBOX_FACTORY_H

#include "linbox/util/error.h"
#include "linbox/blackbox/archetype.h"

namespace LinBox
{

/** Blackbox factory
 *
 * The blackbox factory provides a facility for performing integer or rational
 * computations by reducing modulo one or more primes and recovering the
 * solution with Chinese Remaindering, lifting, or rational reconstruction. It
 * is an interface that provides one method which, given a field, produces a
 * black box representing a particular matrix over that field. The factory
 * object may be passed to various procedures, such as rank, det, and solve,
 * which will perform the required modular reductions to find integer or
 * rational solutions.
 *
 * In the typical case, the user provides an object whose class inherits from
 * BlackboxFactory and implements the method makeBlackbox. The object represents
 * the original integer or rational version of the black box, whose data might
 * require some modification (e.g. modular reduction) to produce a true black
 * box. Alternatively, the resulting black box might merely be a
 * reinterpretation of the data in the original object, as is the case where
 * matrix entries are all nonnegative and smaller than the modulus.
 */

template <class Field, class Vector>
class BlackboxFactory 
{
    public:

	/// Virtual destructor
	virtual ~BlackboxFactory () {}

	/** Given a field and vector type, construct a black box for the matrix
	 * over that field and using that vector type. This should be
	 * implemented by the user
	 */
	virtual BlackboxArchetype<Vector> *makeBlackbox (Field &F) = 0;

}; // BlackboxFactory

} // namespace LinBox

#endif // __BLACKBOX_FACTORY_H
