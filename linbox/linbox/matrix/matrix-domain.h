/* -*- mode: c; style: linux -*- */

/* linbox/field/matrix-domain.h
 * Copyright (C) 2001-2002 Bradford Hovinen
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

#ifndef __FIELD_MATRIX_DOMAIN_H
#define __FIELD_MATRIX_DOMAIN_H

#include <iostream>

#include "linbox/field/archetype.h"
#include "linbox/vector/vector-traits.h"

#include "linbox/debug.h"

#define MatrixDomainType(tag1, tag2) \
	MatrixDomain<Field, Vector1, Vector2, VectorCategories::tag1##VectorTag, VectorCategories::tag2##VectorTag>

#define MatrixDomainSimpleType(tag) \
	MatrixDomain<Field, Vector1, Vector2, VectorCategories::tag##VectorTag>

namespace LinBox
{
	/** Matrix Domain.
	 * Archetype for the matrix domain \Ref{LinBox}.
	 *
	 * This is a generic wrapper around classes matching the
	 * \Ref{Field_archetype} interface. It implements matrix-vector and
	 * vector-vector operations such as axpy, dotprod, and multiDotprod. It
	 * also contains an interface to the underlying field whereby calls
	 * simply pass through. Template specializations permit optimizations to
	 * be done on these operations based on the characteristics of the
	 * field.
	 *
	 * This class is usable by itself. Simply supply any preexisting field
	 * as a template parameter and it will work as intended, though its
	 * operation may not be fully optimized.
	 */
	template <class Field, class Vector1, class Vector2,
		  class Trait1 = VectorTraits<Vector1>::VectorCategory,
		  class Trait2 = VectorTraits<Vector2>::VectorCategory>
	class MatrixDomain
	{
		public:
    
		typedef typename Field::element         element;
		typedef typename Field::RandIter        RandIter;
		typedef vector<element>                 Vector;
		typedef typename Vector::iterator       iterator;
		typedef typename Vector::const_iterator const_iterator;

		/** Copy constructor.
		 * Constructs MatrixDomain_archetype object by copying the domain.
		 * This is required to allow matrix domain objects to be passed
		 * by value into functions.
		 * @param  MD MatrixDomain_archetype object.
		 */
		MatrixDomain (const MatrixDomain &MD) 
			: _F (MD._F)
			{}
    
		/** Assignment operator.
		 * Assigns MatrixDomain object MD to field.
		 * @param  MD MatrixDomain object.
		 */
		MatrixDomain &operator = (const MatrixDomain &MD)
			{ _F = MD._F; return *this; }
    
		/** Assignment of one field element to another.
		 * This function assumes both field elements have already been 
		 * constructed and initialized.
		 * In this implementation, this means for both x and y, 
		 * _elem_ptr exists and does not point to null.
		 * @return reference to x
		 * @param  x field element (reference returned).
		 * @param  y field element.
		 */
		element &assign (element &x, const element &y) const
			{ return _F.assign (x, y); }

		/** Retrieve the underlying field
		 * Return a reference to the field that this matrix domain
		 * object uses
		 * @return reference to field
		 */

		const Field &field () const
			{ return _F; }
    
		//@} Object Management

		/** Vector input/output operations
		 * These routines are useful for reading and writing vectors to
		 * and from file streams. They are analagous to field read and
		 * write operations.
		 */

		//@{

		/** Print vector of field elements.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means for the _elem_ptr for x 
		 * exists and does not point to null.
		 * @return output stream to which field element is written.
		 * @param  os  output stream to which field element is written.
		 * @param  x   field element.
		 */
		ostream &write (ostream &os, const Vector &x) const;

		/** Read vector of field elements.
		 * This function assumes the field element has already been 
		 * constructed and initialized.
		 * In this implementation, this means for the _elem_ptr for x 
		 * exists and does not point to null.
		 * @return input stream from which field element is read.
		 * @param  is  input stream from which field element is read.
		 * @param  x   field element.
		 */
		istream &read (istream &is, Vector &x) const;
    
		//@} Input/Output Operations

		/** @name Vector arithmetic operations
		 * These routes are analogs of field arithmetic operations, but
		 * they take vectors of elements as input. Vector-vector dot
		 * product and vector-vector axpy are supported here.
		 */

		//@{

		/** Vector-vector dot product
		 * @param res element into which to store result
		 * @param v1 Input vector
		 * @param v2 Input vector
		 */
		element &dotprod (element &res, const Vector1 &v1, const Vector2 &v2) const;

		/** Vector axpy
		 * res <- y + a*x
		 * @param res Vector into which to store result
		 * @param y Input vector y
		 * @param a Scalar element a
		 * @param x Input vector x
		 */
		Vector1 &axpy (Vector1 &res, const Vector1 &y, const element &a, const Vector1 &x) const;

		/** Vector in-place axpy
		 * y <- y + a*x
		 * @param y Input vector y; result is stored here
		 * @param a Scalar element a
		 * @param x Input vector x
		 */
		Vector1 &axpyin (Vector1 &y, const element &a, const Vector1 &x) const;
    
		//@} Common Object Interface
    
		/** @name Implementation-Specific Methods.
		 * These methods are not required of all \Ref{LinBox Fields}
		 * and are included only for this implementation of the archetype.
		 */
		//@{

		/** Construct from a field
		 * @param F Field from which to construct
		 */
		MatrixDomain (const Field &F)
			: _F (F)
		{}

		//@} Implementation-Specific Methods
    
	    private:

		Field _F;

	}; // class MatrixDomain

	template <class Field, class Vector1, class Vector2>
	class MatrixDomainType(Dense, Dense) 
	{
	    public:
    
		typedef typename Field::element         element;
		typedef typename Field::RandIter        RandIter;
		typedef vector<element>                 Vector;
		typedef typename Vector::iterator       iterator;
		typedef typename Vector::const_iterator const_iterator;

		MatrixDomain (const MatrixDomain &MD) : _F (MD._F)                  {}
		MatrixDomain &operator = (const MatrixDomain &MD)                   { _F = MD._F; return *this; }
    		element &init (element &x, const integer &y = 0 ) const             { return _F.init (x, y); }
  		integer &convert (integer &x, const element &y = 0) const           { return _F.convert (x, y); }
  		element &assign (element &x, const element &y) const                { return _F.assign (x, y); }
  		integer &cardinality (integer &c) const                             { return _F.cardinality (c); }
  		integer &characteristic (integer &c) const                          { return _F.characteristic (c); }
  		bool areEqual (const element &x, const element &y) const            { return _F.areEqual (*x, *y); }
  		element &add (element &x, const element &y, const element &z) const { return _F.add (x, y, z); }
  		element &sub (element &x, const element &y, const element &z) const { return _F.sub (x, y, z); }
  		element &mul (element &x, const element &y, const element &z) const { return _F.mul (x, y, z); }
		element &div (element &x, const element &y, const element &z) const { return _F.div (x, y, z); }
		element &neg (element &x, const element &y) const                   { return _F.neg (x, y); }
		element &inv (element &x, const element &y) const                   { return _F.inv (x, y); }
		element &axpy (element &r, const element &a, const element &x, const element &y) const
			                                                            { return _F.axpy (r, a, x, y); }
		bool isZero (const element &x) const                                { return _F.isZero (x); }
		bool isOne (const element &x) const                                 { return _F.isOne (x); }
		element &addin (element &x, const element &y) const                 { return _F.addin (x, y); }
		element &subin (element &x, const element &y) const                 { return _F.subin (x, y); }
		element &mulin (element &x, const element &y) const                 { return _F.mulin (x, y); }
		element &divin (element &x, const element &y) const                 { return _F.divin (x, y); }
		element &negin (element &x) const                                   { return _F.negin (x); }
		element &invin (element &x) const                                   { return _F.invin (x); }
		element &axpyin (element &r, const element &a, const element &x) const { return _F.axpyin (r, a, x); }
		ostream &write (ostream &os) const                                  { return _F.write (os); }
		istream &read (istream &is)                                         { return _F.read (is); }
		ostream &write (ostream &os, const element &x) const                { return _F.write (os, x); }
		istream &read (istream &is, element &x) const                       { return _F.read (is, x); }
		MatrixDomain (const Field &F) : _F (F)                              {}
		element &dotprod (element &res, const Vector1 &v1, const Vector2 &v2) const;
		Vector1 &axpy (Vector1 &res, const Vector1 &y, const element &a, const Vector1 &x) const;
		Vector1 &axpyin (Vector1 &y, const element &a, const Vector1 &x) const;

	    private:

		Field _F;
	};

	template <class Field, class Vector1, class Vector2>
	class MatrixDomainType(SparseSequence, Dense) 
	{
	    public:
    
		typedef typename Field::element         element;
		typedef typename Field::RandIter        RandIter;
		typedef vector<element>                 Vector;
		typedef typename Vector::iterator       iterator;
		typedef typename Vector::const_iterator const_iterator;

		MatrixDomain (const MatrixDomain &MD) : _F (MD._F)                  {}
		MatrixDomain &operator = (const MatrixDomain &MD)                   { _F = MD._F; return *this; }
    		element &init (element &x, const integer &y = 0 ) const             { return _F.init (x, y); }
  		integer &convert (integer &x, const element &y = 0) const           { return _F.convert (x, y); }
  		element &assign (element &x, const element &y) const                { return _F.assign (x, y); }
  		integer &cardinality (integer &c) const                             { return _F.cardinality (c); }
  		integer &characteristic (integer &c) const                          { return _F.characteristic (c); }
  		bool areEqual (const element &x, const element &y) const            { return _F.areEqual (*x, *y); }
  		element &add (element &x, const element &y, const element &z) const { return _F.add (x, y, z); }
  		element &sub (element &x, const element &y, const element &z) const { return _F.sub (x, y, z); }
  		element &mul (element &x, const element &y, const element &z) const { return _F.mul (x, y, z); }
		element &div (element &x, const element &y, const element &z) const { return _F.div (x, y, z); }
		element &neg (element &x, const element &y) const                   { return _F.neg (x, y); }
		element &inv (element &x, const element &y) const                   { return _F.inv (x, y); }
		element &axpy (element &r, const element &a, const element &x, const element &y) const
			                                                            { return _F.axpy (r, a, x, y); }
		bool isZero (const element &x) const                                { return _F.isZero (x); }
		bool isOne (const element &x) const                                 { return _F.isOne (x); }
		element &addin (element &x, const element &y) const                 { return _F.addin (x, y); }
		element &subin (element &x, const element &y) const                 { return _F.subin (x, y); }
		element &mulin (element &x, const element &y) const                 { return _F.mulin (x, y); }
		element &divin (element &x, const element &y) const                 { return _F.divin (x, y); }
		element &negin (element &x) const                                   { return _F.negin (x); }
		element &invin (element &x) const                                   { return _F.invin (x); }
		element &axpyin (element &r, const element &a, const element &x) const { return _F.axpyin (r, a, x); }
		ostream &write (ostream &os) const                                  { return _F.write (os); }
		istream &read (istream &is)                                         { return _F.read (is); }
		ostream &write (ostream &os, const element &x) const                { return _F.write (os, x); }
		istream &read (istream &is, element &x) const                       { return _F.read (is, x); }
		MatrixDomain (const Field &F) : _F (F)                              {}
		element &dotprod (element &res, const Vector1 &v1, const Vector2 &v2) const;
		Vector1 &axpy (Vector1 &res, const Vector1 &y, const element &a, const Vector1 &x) const;
		Vector1 &axpyin (Vector1 &y, const element &a, const Vector1 &x) const;

	    private:

		Field _F;
	};

	template <class Field, class Vector1, class Vector2>
	class MatrixDomainSimpleType(Dense) 
	{
	    public:
    
		typedef typename Field::element         element;
		typedef typename Field::RandIter        RandIter;
		typedef vector<element>                 Vector;
		typedef typename Vector::iterator       iterator;
		typedef typename Vector::const_iterator const_iterator;

		MatrixDomain (const MatrixDomain &MD) : _F (MD._F)                  {}
		MatrixDomain &operator = (const MatrixDomain &MD)                   { _F = MD._F; return *this; }
    		element &init (element &x, const integer &y = 0 ) const             { return _F.init (x, y); }
  		integer &convert (integer &x, const element &y = 0) const           { return _F.convert (x, y); }
  		element &assign (element &x, const element &y) const                { return _F.assign (x, y); }
  		integer &cardinality (integer &c) const                             { return _F.cardinality (c); }
  		integer &characteristic (integer &c) const                          { return _F.characteristic (c); }
  		bool areEqual (const element &x, const element &y) const            { return _F.areEqual (*x, *y); }
  		element &add (element &x, const element &y, const element &z) const { return _F.add (x, y, z); }
  		element &sub (element &x, const element &y, const element &z) const { return _F.sub (x, y, z); }
  		element &mul (element &x, const element &y, const element &z) const { return _F.mul (x, y, z); }
		element &div (element &x, const element &y, const element &z) const { return _F.div (x, y, z); }
		element &neg (element &x, const element &y) const                   { return _F.neg (x, y); }
		element &inv (element &x, const element &y) const                   { return _F.inv (x, y); }
		element &axpy (element &r, const element &a, const element &x, const element &y) const
			                                                            { return _F.axpy (r, a, x, y); }
		bool isZero (const element &x) const                                { return _F.isZero (x); }
		bool isOne (const element &x) const                                 { return _F.isOne (x); }
		element &addin (element &x, const element &y) const                 { return _F.addin (x, y); }
		element &subin (element &x, const element &y) const                 { return _F.subin (x, y); }
		element &mulin (element &x, const element &y) const                 { return _F.mulin (x, y); }
		element &divin (element &x, const element &y) const                 { return _F.divin (x, y); }
		element &negin (element &x) const                                   { return _F.negin (x); }
		element &invin (element &x) const                                   { return _F.invin (x); }
		element &axpyin (element &r, const element &a, const element &x) const { return _F.axpyin (r, a, x); }
		ostream &write (ostream &os) const                                  { return _F.write (os); }
		istream &read (istream &is)                                         { return _F.read (is); }
		ostream &write (ostream &os, const element &x) const                { return _F.write (os, x); }
		istream &read (istream &is, element &x) const                       { return _F.read (is, x); }
		MatrixDomain (const Field &F) : _F (F)                              {}
		element &dotprod (element &res, const Vector1 &v1, const Vector2 &v2) const;
		Vector1 &axpy (Vector1 &res, const Vector1 &y, const element &a, const Vector1 &x) const;
		Vector1 &axpyin (Vector1 &y, const element &a, const Vector1 &x) const;

	    private:

		Field _F;
	};

	template <class Field, class Vector1, class Vector2>
	class MatrixDomainSimpleType(SparseSequence) 
	{
	    public:
    
		typedef typename Field::element         element;
		typedef typename Field::RandIter        RandIter;
		typedef vector<element>                 Vector;
		typedef typename Vector::iterator       iterator;
		typedef typename Vector::const_iterator const_iterator;

		MatrixDomain (const MatrixDomain &MD) : _F (MD._F)                  {}
		MatrixDomain &operator = (const MatrixDomain &MD)                   { _F = MD._F; return *this; }
    		element &init (element &x, const integer &y = 0 ) const             { return _F.init (x, y); }
  		integer &convert (integer &x, const element &y = 0) const           { return _F.convert (x, y); }
  		element &assign (element &x, const element &y) const                { return _F.assign (x, y); }
  		integer &cardinality (integer &c) const                             { return _F.cardinality (c); }
  		integer &characteristic (integer &c) const                          { return _F.characteristic (c); }
  		bool areEqual (const element &x, const element &y) const            { return _F.areEqual (*x, *y); }
  		element &add (element &x, const element &y, const element &z) const { return _F.add (x, y, z); }
  		element &sub (element &x, const element &y, const element &z) const { return _F.sub (x, y, z); }
  		element &mul (element &x, const element &y, const element &z) const { return _F.mul (x, y, z); }
		element &div (element &x, const element &y, const element &z) const { return _F.div (x, y, z); }
		element &neg (element &x, const element &y) const                   { return _F.neg (x, y); }
		element &inv (element &x, const element &y) const                   { return _F.inv (x, y); }
		element &axpy (element &r, const element &a, const element &x, const element &y) const
			                                                            { return _F.axpy (r, a, x, y); }
		bool isZero (const element &x) const                                { return _F.isZero (x); }
		bool isOne (const element &x) const                                 { return _F.isOne (x); }
		element &addin (element &x, const element &y) const                 { return _F.addin (x, y); }
		element &subin (element &x, const element &y) const                 { return _F.subin (x, y); }
		element &mulin (element &x, const element &y) const                 { return _F.mulin (x, y); }
		element &divin (element &x, const element &y) const                 { return _F.divin (x, y); }
		element &negin (element &x) const                                   { return _F.negin (x); }
		element &invin (element &x) const                                   { return _F.invin (x); }
		element &axpyin (element &r, const element &a, const element &x) const { return _F.axpyin (r, a, x); }
		ostream &write (ostream &os) const                                  { return _F.write (os); }
		istream &read (istream &is)                                         { return _F.read (is); }
		ostream &write (ostream &os, const element &x) const                { return _F.write (os, x); }
		istream &read (istream &is, element &x) const                       { return _F.read (is, x); }
		MatrixDomain (const Field &F) : _F (F)                              {}
		element &dotprod (element &res, const Vector1 &v1, const Vector2 &v2) const;
		Vector1 &axpy (Vector1 &res, const Vector1 &y, const element &a, const Vector1 &x) const;
		Vector1 &axpyin (Vector1 &y, const element &a, const Vector1 &x) const;

	    private:

		Field _F;
	};

} // namespace LinBox

#include "linbox/field/matrix-domain.C"

#endif // __FIELD_MATRIX_DOMAIN_H
