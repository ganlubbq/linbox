/* linbox/matrix/blas-vector.h
 * Copyright (C) 2013 the LinBox group
 *
 * Written by :
 * BB <bbboyer@ncsu.edu>
 *
 * ========LICENCE========
 * This file is part of the library LinBox.
 *
  * LinBox is free software: you can redistribute it and/or modify
 * it under the terms of the  GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========
 */


/*! @file vector/blas-vector.h
 * @ingroup vector
 * A \c BlasVector<\c _Field > represents a vector as an array of
 * <code>_Field::Element</code>s.
 *
 */

#ifndef __LINBOX_vector_blas_vector_H
#define __LINBOX_vector_blas_vector_H

#include "linbox/util/debug.h"
#include "linbox/linbox-tags.h"
#include "linbox/vector/subiterator.h"
#include "linbox/vector/subvector.h"
// #include "linbox/field/hom.h"

namespace LinBox { /* BlasVector */

	template<class _Vector>
	class BlasSubvector ;

	template<class _Field, class _Rep>
	class BlasVector ;

	template<class _Field, class _Rep>
	class BlasMatrix ;

	template<class _Matrix>
	class BlasSubmatrix ;

	template <class Field, class _Rep>
	struct VectorTraits< BlasVector<Field,_Rep> > {
		typedef typename VectorCategories::DenseVectorTag VectorCategory;
		typedef BlasVector<Field,_Rep>                          VectorType;
	};


	template<class _Field, class _blasRep=typename RawVector<typename _Field::Element>::Dense>
	class BlasVector : public Subvector<Subiterator<typename _blasRep::iterator > >
	{

	public:
		typedef _Field                                Field;
		typedef typename Field::Element             Element;    //!< Element type
		typedef _blasRep                                Rep;    //!< Actually a <code>std::vector<Element></code> (or alike.)
		typedef typename Rep::pointer               pointer;    //!< pointer type to elements
		typedef const pointer                 const_pointer;    //!< const pointer type
		typedef BlasVector<_Field,_blasRep>          Self_t;    //!< Self type
		typedef BlasSubvector<Self_t>         subVectorType;    //!< Submatrix type
		typedef BlasVector<_Field,_blasRep>      vectorType;    //!< matrix type
		typedef BlasVector<_Field,_blasRep>        blasType;    //!< blas type

	public: /* iterators */
		typedef Subvector<Subiterator<typename _blasRep::iterator > > Father_t;
		typedef typename Father_t::iterator iterator;
		typedef typename Father_t::const_iterator const_iterator;
		// typedef typename Father_t::size_type size_type;



	protected:
		size_t			       _size;
		size_t                       _1stride;
		Rep			        _rep;
		pointer			        _ptr;
		const Field		    * _field;
	private:
		void createBlasVector(const BlasVector<_Field,_blasRep> & V)
		{
			//! use std::copy somehow ?
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,V.getEntry(i));
		}

		void createBlasVector(const BlasVector<_Field,_blasRep> & V
				      , const size_t i0, const size_t str)
		{
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,V.getEntry(i0+i*str));
		}

		template<class _Vector>
		void createBlasVector(const BlasSubvector<_Vector> & V)
		{
			//! use std::copy somehow ?
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,V.getEntry(i));
		}

		void createBlasVector ( const Element * V)
		{
			//! use std::copy somehow ?
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,V[i]);
		}

		void createBlasVector ( const Element * V
					, const size_t i0, const size_t str)
		{
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,V[i0+i*str]);
		}

		void createBlasVector ( const std::vector<Element> & V)
		{
			createBlasVector(&V[0]);
		}

		template<class OtherVector>
		void createBlasVector( const OtherVector &V)
		{
			iterator it = _rep.begin();
			typename OtherVector::const_iterator jt = V.begin();
			for ( ; it != _rep.end(); ++it, ++jt)
				*it = *jt ;

		}

		void createBlasVector ( const std::vector<Element> & V
					, const size_t i0, const size_t str)
		{
			createBlasVector(&V[0],i0,str);
		}

		/** Gets a vector from data in a BlasMatrix.
		 * Vector starts at \c (i0,j0) and next element is \p stride away.
		 * For instance, row \c i correpsonds to args \c (i,0,1) and col \c j is \c (0,j,lda)
		 * It is possible to get vectors that extend over multiple rows/columns
		 * and \p stride need not be equal to \c 1 or \c lda.
		 */
		void createBlasVector ( const BlasMatrix<Field,Rep> & A, size_t i0, size_t j0, size_t str)
		{
			if ((str == 1) && (i0+_size*str<A.coldim())){
				for (size_t i = 0 ; i < _size ; ++i)
					setEntry(i,A.getEntry(i0+i,j0));
			}
			else if ((str == A.coldim()) && j0+_size*str<A.rowdim()) {
				for (size_t i = 0 ; i < _size ; ++i)
					setEntry(i,A.getEntry(i0,j0+i));
			}
			else{ /* is this always faster ? */
				Element * Aptr = A.getPointer(i0,j0);
				for (size_t i = 0 ; i < _size ; ++i)
					setEntry(i,Aptr[i*str]);
			}
		}

		template<class _Matrix>
		void createBlasVector ( const BlasSubmatrix<_Matrix> & A, size_t i0, size_t j0, size_t str)
		{
			Element * Aptr = A.getPointer(i0,j0);
			for (size_t i = 0 ; i < _size ; ++i)
				setEntry(i,Aptr[i*str]);
		}


	public:
		BlasVector () {} ;

		BlasVector (const _Field &F)  :
			_size(0),_1stride(1),_rep(0),_ptr(&_rep[0]), _field(&F)
		{
			// Father_t is garbage until then:
			setIterators();

			// linbox_check(_ptr != NULL);
		}

#if 0
		void init(const _Field & F, size_t n = 0)
		{
			_field = &F;
			_size = n;
			_1stride=1 ;
			_rep.resize(n, F.zero);
			_ptr = &_rep[0];
		}
#endif

#ifdef __GNUC__
#ifndef __x86_64__
#if (__GNUC__ == 4 && __GNUC_MINOR__ ==4 && __GNUC_PATCHLEVEL__==5)
		BlasVector (const _Field &F, const long &m, const Element e=Element()) :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
			// Father_t is garbage until then:
			setIterators();

			linbox_check(_size==0 || _ptr != NULL);
		}
#endif
#endif
#endif

#if defined(__APPLE__) || (defined(__s390__) && !defined(__s390x__))
		BlasVector (const _Field &F, const unsigned long &m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
			// Father_t is garbage until then:
			setIterators();

			linbox_check(_size==0 || _ptr != NULL);
			linbox_check(_size >= this->begin()->_stride);
		}

#endif

		BlasVector (const _Field &F, const uint64_t &m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
			// Father_t is garbage until then:
			setIterators();
			// linbox_check(_ptr != NULL);
		}


		BlasVector (const _Field &F, const int64_t &m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
	// Father_t is garbage until then:
			setIterators();


			linbox_check(_size==0 || _ptr != NULL);
		}


		BlasVector (const _Field &F, const uint32_t &m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
	// Father_t is garbage until then:
			setIterators();


			linbox_check(_size==0 || _ptr != NULL);
		}

		BlasVector (const _Field &F, const int32_t &m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
	// Father_t is garbage until then:
			setIterators();


			linbox_check(_size==0 || _ptr != NULL);
		}

		BlasVector (const _Field &F, const Integer & m, const Element e=Element())  :
			_size((size_t)m),_1stride(1),_rep((size_t)_size, e),_ptr(&_rep[0]),_field(&F)
		{
	// Father_t is garbage until then:
			setIterators();


			linbox_check(_size==0 || _ptr != NULL);
		}

		//! @bug be careful with copy constructor. We should ban them and provide copy.
		BlasVector (const BlasVector<_Field,_blasRep> &V)  :
			_size(V.size())
			,_1stride(1)
			,_rep(V.size()/*, V.field().zero*/) //!@bug segfault in cra otherwise (test-rat-solve eg)
			,_ptr(&_rep[0])
			,_field(&(V.field()))
			,Father_t() // will be created afterwards...
		{
			// Father_t is garbage until then:
			setIterators();

			createBlasVector(V);

			linbox_check(_size==0 || _ptr != NULL);
		}

		template<class VectorBase>
		BlasVector (const _Field & F, const VectorBase & V)  :
			_size(V.size()),_1stride(1),_rep(V.size(), F.zero),_ptr(&_rep[0]),_field(&F)
			,Father_t() // will be created afterwards...
		{
			// Father_t is garbage until then:
			setIterators();

			createBlasVector(V);

			linbox_check(_size==0 || _ptr != NULL);
		}


		template<class _Vector>
		BlasVector (const BlasSubvector<_Vector> &V)  :
			_size(V.size()),_1stride(1),_rep(V.size(), V.field().zero),_ptr(&_rep[0]),_field(&(V.field()))
		{
	// Father_t is garbage until then:
			setIterators();


			createBlasVector(V);
			linbox_check(_size==0 || _ptr != NULL);
		}

		BlasVector (const BlasMatrix<Field,Rep> &A, size_t k, LINBOX_enum (Tag::Direction) f )  :
			_size((f == Tag::Direction::Row)?(A.rowdim()):(A.coldim())),_1stride(1),_rep(_size, A.field().zero),_ptr(&_rep[0]),_field(&(A.field()))
			{
	// Father_t is garbage until then:
			setIterators();


				if (f==Tag::Direction::Row)
					createBlasVector(A,k,0,1);
				else // Tag::Col
					createBlasVector(A,0,k,A.coldim());
			linbox_check(_size==0 || _ptr != NULL);

			}

		template<class _Matrix>
		BlasVector (const BlasSubmatrix<_Matrix> &A, size_t k, LINBOX_enum (Tag::Direction) f )  :
			_size((f==Tag::Direction::Row)?(A.rowdim()):(A.coldim())),_1stride(1),_rep(_size, A.field().zero),_ptr(&_rep[0]),_field(&(A.field()))
			{
	// Father_t is garbage until then:
			setIterators();


				if (f==Tag::Direction::Row)
					createBlasVector(A,k,0,1);
				else // Tag::Col
					createBlasVector(A,0,k,A.stride());
			linbox_check(_size==0 || _ptr != NULL);
			}

		BlasVector (const BlasMatrix<Field,Rep> &A, size_t n, size_t i0, size_t j0, size_t str )  :
			_size(n),_1stride(1),_rep(_size, A.field().zero),_ptr(&_rep[0]),_field(&(A.field()))
		{
	// Father_t is garbage until then:
			setIterators();


			createBlasVector(A,i0,j0,str);
			linbox_check(_size==0 || _ptr != NULL);
		}

		BlasVector(const _Field & F, const typename _Field::Element * v, const size_t l) :
			_size(l),_1stride(1),_rep(l, F.zero),_ptr(&_rep[0]),_field(&F)
		{
			setIterators();
			createBlasVector(v);
			linbox_check(_size==0 || _ptr != NULL);
		}

		// ~BlasVector () ;

		BlasVector<_Field,_blasRep>& operator= (const BlasVector<_Field,_blasRep>& V)
		{
			if ( &V == this)
				return *this;

			_size = V.size();
			_1stride = 1;
			_rep = Rep(_size);
			_ptr = &_rep[0] ;
			createBlasVector(V);
			linbox_check(_size==0 || _ptr != NULL);

			// Father_t is garbage until then:
			setIterators();



			return *this;
		}

		//! this should not exist.
		BlasVector<_Field,_blasRep>& operator= (const std::vector<Element>& V)
		{
			_size = V.size();
			_1stride = 1;
			_rep = Rep(_size);
			_ptr = &_rep[0] ;
			createBlasVector(V);
			linbox_check(_size==0 || _ptr != NULL);

			// Father_t is garbage until then:
			setIterators();



			return *this;
		}

		template<class OtherBase>
		BlasVector<_Field,_blasRep>& operator= (const OtherBase & V)
		{
			_size = V.size();
			_1stride = 1;
			_rep = Rep(_size);
			_ptr = &_rep[0] ;
			createBlasVector(V);
			linbox_check(_size==0 || _ptr != NULL);

			// Father_t is garbage until then:
			setIterators();

			return *this;
		}

		template<typename _Tp1>
		struct rebind {
			typedef BlasVector<_Tp1> other;

#if 0 /* problem with hom.h */
			void operator() (other & Ap, const Self_t& A) {
				typedef typename Self_t::ConstIterator ConstSelfIterator ;
				typedef typename other::Iterator OtherIterator ;
				OtherIterator    Ap_i = Ap.Begin();
				ConstSelfIterator A_i = A.Begin();
				Hom<Field, _Tp1> hom(A. field(), Ap. field()) ;
				for ( ; A_i != A. End(); ++ A_i, ++ Ap_i)
					hom.image (*Ap_i, *A_i);
			}
#endif
		};


		// write
		std::ostream &write ( std::ostream &os, LINBOX_enum(Tag::FileFormat) fmt = Tag::FileFormat::Pretty ) const
		{
			switch(fmt) {
			case (Tag::FileFormat::Pretty) :
				{
					os << '[' ;
					for(const_iterator it= this->begin();it != this->end(); ++it)
						field().write(os, *it) << " ";
					return	os << ']' ;
				}
			case (Tag::FileFormat::Maple) :
				{
					os << '<' ;
					for(const_iterator it=this->begin();it != this->end(); ) {
						field().write(os, *it);
						++it ;
						if (it != this->end())
							os << ',' ;
					}
					return	os << '>' ;
				}

			default :
				return os << "not implemented" ;
			}
		}



		// read

			size_t size() const
		{
			return _size;
		}

		//!should never ever be used
		size_t getStride() const
		{
			return _1stride;
		}

		size_t stride() const { return getStride() ;}

		//!should never ever be used
		size_t& getWriteStride()
		{
			return _1stride;
		}

		void resize (size_t n, const Element& val = Element())
		{
			_size = n;
			_rep.resize (n, val);
			_ptr=&_rep[0];

			// iterators are changed
			setIterators();

		}
		// should use this->insert(this->end(),e)
		void push_back(const Element & e) {
			_rep.push_back(e);

			_size = _rep.size() ; // back to normal :-)
			_ptr=&_rep[0];
			setIterators();

		}

		void clear(void) {
			_rep.clear();
			_size = 0 ;
			_ptr=&_rep[0]; // probably NULL
			setIterators();

		}

		void reserve(const size_t &m) {
			_rep.reserve(m);
			// _size = _rep.size() ;
			_ptr=&_rep[0]; // do we need those ?
			setIterators();

		}



		Rep & refRep() { return _rep ; }

		Rep getRep() const { return _rep ; }

		pointer getPointer() const
		{
			return _ptr;
		}

		const_pointer &getConstPointer() const
		{
			return (const_pointer)_ptr;
		}

		pointer& getWritePointer()  //! @bug should be called refPointer()
		{
			return _ptr;
		}

		void setEntry (size_t i, const Element &a_i)
		{
			_ptr[i] = a_i;
		}

		Element &refEntry (size_t i)
		{
			return _ptr[i];
		}

		const Element &getEntry (size_t i) const
		{
			return _ptr[i];
		}

		Element &getEntry (Element &x, size_t i) const
		{
			x = _ptr[i];
			return x;
		}

		void random()
		{
			typename _Field::Element x; field().init(x);
			typename _Field::RandIter r(field());
			for (size_t i = 0; i < size(); ++i)
				setEntry(i, r.random(x));
		}


		const _Field& field() const { return const_cast<Field&>( *_field );}
		//
		//
		// _Field & field() { return const_cast<Field&>(*_field;) }
		void changeField(const Field & G) {
			_field = const_cast<Field*>(&G) ;
		}

	private:
		void setIterators()
		{
			Father_t::_begin = iterator (_rep.begin() , 1);
			Father_t::_end = iterator (_rep.begin()+(ptrdiff_t)_size , 1);
		}


	};// BlasVector

	template<class T>
	std::ostream& operator<< (std::ostream & o, const BlasVector<T> & Mat)
	{
		return Mat.write(o);
	}

} // LinBox

namespace LinBox { /*  BlasSubvector */




	template <class _Vector > // inherit that from owner ?
	class BlasSubvector {
	public :
		typedef typename _Vector::Field                   Field;
		typedef typename Field::Element                 Element;      //!< Element type
		typedef typename _Vector::Rep                       Rep;    //!< Actually a <code>std::vector<Element></code> (or alike.)
		typedef BlasSubvector<_Vector>                   Self_t;       //!< Self type
		typedef typename Rep::pointer                   pointer;    //!< pointer type to elements
		typedef const pointer                     const_pointer;    //!< const pointer type
		typedef Self_t                            subVectorType;    //!< Subvector type
		typedef BlasVector<Field,Rep>                vectorType;    //!< vector type
		typedef BlasVector<Field,Rep>                  blasType;    //!< blas type


	protected:
		Rep &_Vec;                     //!< Parent raw vector
		size_t _size;                   //!< size of Subvector
		size_t _i0;                    //!< beginning of Subvector in \p _Vec
		size_t _1stride ;               //!< number of columns in \p _Vec (or stride of \p _Vec)
		Field & _field;

	public:

		//////////////////
		// CONSTRUCTORS //
		//////////////////


		/*  constructors */

		/** NULL constructor.  */
		// BlasSubvector () ;

		BlasSubvector (const BlasVector<Field,Rep> &V,
			       size_t ibeg,
			       size_t siz,
			       size_t str
			      ) :
			_Vec (const_cast<Rep&>(V.refRep())),
			_size(siz),_i0 (ibeg),_1stride(str)
			,_field(V.field())
		{}


		BlasSubvector (const BlasVector<Field,Rep> &V) :
			_Vec (const_cast<Rep&>(V.refRep())),
			_size(V.size()),_i0 (0),_1stride(1)
			,_field(V.field())
		{}



		BlasSubvector (const BlasSubvector<_Vector> &SV,
			       size_t ibeg,
			       size_t siz
			       // , size_t stride //!stride in new subvector as if SV were of stride 1
			      ) :
			_Vec (SV._Vec),
			_size(siz),_i0 (SV._i0*SV._1stride+ibeg)
			,_1stride(SV._1stride)
			// ,_1stride(SV._1stride*stride)
			,_field(SV.field())
		{}


		/** Copy constructor.
		 * @param SM Subvector to copy
		 */
		BlasSubvector (const BlasSubvector<_Vector> &SV) :
			_Vec (SV._Vec),
			_size(SV._size),_i0 (SV._i0)
			,_1stride(SV._1stride)
			,_field(SV.field())
		{}

		BlasSubvector (const BlasMatrix<Field,Rep> &M
			       , size_t beg
			       , LINBOX_enum (Tag::Direction) f ) :
			_Vec (const_cast<Rep&>(M.refRep()))
			,_size((f==Tag::Direction::Row)?(M.coldim()):(M.rowdim()))
			,_i0 ((f==Tag::Direction::Row)?(beg*M.coldim()):(beg))
			,_1stride((f==Tag::Direction::Row)?(1):(M.coldim()))
			,_field(M.field())
			{}

		template<class _Matrix>
		BlasSubvector (const BlasSubmatrix<_Matrix> &M
			       , size_t beg
			       , LINBOX_enum (Tag::Direction) f ) :
			_Vec (const_cast<Rep&>(M.refRep()))
			,_size((f==Tag::Direction::Row)?(M.coldim()):(M.rowdim()))
			,_i0 ((f==Tag::Direction::Row)?(M.offset()+beg*M.stride()):(M.offset()+beg))
			,_1stride((f==Tag::Direction::Row)?(1):(M.stride()))
			,_field(M.field())
			{}


		//! @todo more general subvectors

		/*  Members  */

		BlasSubvector &operator = (const BlasSubvector<_Vector> &SV)
		{
			if ( &SV == this)
				return *this ;
			_Vec=SV._Vec ; //!@todo use functions
			_size = SV.size();
			_i0=SV._i0;
			_1stride = SV.stride();
			_field = SV.field();
			return *this ;

		}

		// template<typename _Tp1>
		// struct rebind ;

		//////////////////
		//  DIMENSIONS  //
		//////////////////

		size_t size() const {return _size;}

		size_t getStride() const {return _1stride ; }
		size_t stride() const { return getStride() ;}


		///////////////////
		//      I/O      //
		///////////////////

		// template<class Field>
		// std::istream& read (std::istream &file/*, const Field& field*/);


		// std::ostream &write (std::ostream &os) const;


		//////////////////
		//   ELEMENTS   //
		//////////////////

		pointer getPointer() const { return &(_Vec[_i0]); }

		const_pointer &getConstPointer() const { return &(_Vec[_i0]); }


		pointer& getWritePointer() { return &(_Vec[_i0]); }


		void setEntry (size_t i, const Element &a_i)
		{
			_Vec[_i0+i*_1stride] = a_i;
		}

		Element &refEntry (size_t i)
		{
			return _Vec[_i0+i*_1stride] ;
		}

		const Element &getEntry (size_t i) const
		{
			return _Vec[_i0+i*_1stride] ;
		}

		Element &getEntry (Element &x, size_t i) const
		{
			x = _Vec[_i0+i*_1stride] ;
			return x;
		}


		///////////////////
		//   ITERATORS   //
		///////////////////

		// class Iterator  ;
		// class ConstIterator ;

		// class IndexedIterator ;
		// class ConstIndexedIterator ;


		const Field& field() const { return _field ;}
		Field & field() { return _field; }
	};

	template<class T>
	std::ostream& operator<< (std::ostream & o, const BlasSubvector<T> & Mat)
	{
		return Mat.write(o);
	}

} // LinBox



#include "blas-vector.inl"

#endif // __LINBOX_vector_blas_vector_H

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 8
// End:
// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
