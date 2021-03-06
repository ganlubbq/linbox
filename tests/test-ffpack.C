/* Copyright (C) LinBox
 *
 * written by C. Pernet
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


/*! @file tests/test-ffpack.C
 * @brief Tests for the ffpack set of routines.
 * usage: test-ffpack p A n, for n lsp factorization  of A over Z/pZ
 *
 * \ingroup tests
 * @bug this file does not compile or the test fails for some fields.
 * @test NO DOC
 */

#include "linbox/linbox-config.h"

#include <iostream>


#include "linbox/integer.h"
#include "linbox/matrix/matrix-domain.h"
//#include "linbox/field/givaro.h"
#include "linbox/ring/modular.h"
#include <givaro/modular-balanced.h>
#include "fflas-ffpack/ffpack/ffpack.h"
#include "linbox/util/commentator.h"

#include "test-common.h"

#define _LB_FULL_TEST

using namespace LinBox;

string blank;

const int maxpretty = 35;
const char* pretty(string a)
{

	blank = "     " + a;
	int msgsize= maxpretty - (int)blank.size();
	string dot(".");
	for (int i=0;i<msgsize ;++i)
		blank+=dot;
	return blank.c_str();
}

/*
 *  Testing the rank of dense matrices
 *  construct a n*n matrices of rank r and compute the rank
 */
template <class Field>
static bool testRank (const Field& F,size_t n, int iterations)
{

	typedef typename Field::Element Element;
	typedef typename Field::RandIter RandIter;
	typedef typename Field::NonZeroRandIter NonZeroRandIter;

	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing rank"),"testRank",(unsigned int)iterations);

	RandIter G(F);
	NonZeroRandIter Gn(G);

	bool ret = true;

	for (int k=0;k<iterations; ++k) {
		unsigned int r;

		commentator().progress(k);
		Element * A = new Element[n*n];
		Element * S = new Element[n*n];
		Element * L = new Element[n*n];

		r = (unsigned)( (size_t)rand() % n );
		// create S as an upper triangular matrix with r nonzero rows
		for (size_t i=0;i<r;++i){
			for (size_t j=0;j<i;++j)
				F.assign(*(S+j+i*n),F.zero);
			Gn.random(*(S+i*n+i));
			for (size_t j=i+1;j<n;++j)
				G.random(*(S+i*n+j));
		}
		for (size_t i=r;i<n;++i){
			for (size_t j=0;j<n;++j)
				F.assign(*(S+j+i*n),F.zero);
		}
		// create L as a lower triangular matrix with nonzero elements on the diagonal
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				G.random(*(L+i*n+j));
			Gn.random(*(L+i*n+i));
			for (size_t j=i+1;j<n;++j)
				F.assign(*(L+j+i*n),F.zero);

		}

		//  compute A=LS
		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, n, n, n,
			      F.one, L, n, S, n, F.zero, A, n );
		delete[] L;
		delete[] S;

		// compute the rank of A
		unsigned int rank= (unsigned int) FFPACK::Rank( F, n, n, A, n);
                delete[] A;
		if (rank!=r)
			ret=false;
	}

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testRank");

	return ret;
}


/*
 *  Testing the determinant of dense matrices using BlasDomain
 *  construct a n*n matrices of determinant d and compute the determinant
 */
template <class Field>
static bool testDet (const Field& F,size_t n, int iterations)
{

	typedef typename Field::Element Element;
	typedef typename Field::RandIter RandIter;
	typedef typename Field::NonZeroRandIter NonZeroRandIter;

	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing determinant"),"testDet",(unsigned int)iterations);

	Element d;
	RandIter G(F);
	NonZeroRandIter Gn(G);

	bool ret = true;

	for (int k=0;k<iterations;++k) {

		commentator().progress(k);

		G.random(d);

		Element * A = new Element[n*n];
		Element * S = new Element[n*n];
		Element * L = new Element[n*n];



		// create S as an upper triangular matrix of full rank
		// with diagonal's element equal to 1 except the first entry wich equals to d
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				F.assign(*(S+j+i*n),F.zero);
			F.assign(*(S+i*n+i), F.one);
			for (size_t j=i+1;j<n;++j)
				G.random(*(S+i*n+j));
		}
		F.assign(*S,d);

		// create L as a lower triangular matrix with only 1's on diagonal
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				G.random(*(L+i*n+j));
			F.assign(*(L+i*n+i),F.one);
			for (size_t j=i+1;j<n;++j)
				F.assign(*(L+j+i*n),F.zero);
		}


		//  compute A=LS
		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, n, n, n,
			      F.one, L, n, S, n, F.zero, A, n );
		delete[] L;
		delete[] S;


		// compute the determinant of A
		Element det= FFPACK::Det( F, n, n, A, n);
    		delete[] A;


		if (!F.areEqual(det,d))
			ret=false;
	}

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testDet");

	return ret;
}

/*
 * Test of the LQUP factorization routine
 */
template <class Field>
static bool testLUdivine (const Field& F, size_t m, size_t n, int iterations)
{

	typedef typename Field::Element                  Element;
	typedef typename Field::RandIter                RandIter;
	typedef typename Field::NonZeroRandIter                NonZeroRandIter;

	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing LQUP factorization"),"testLQUP",(unsigned int)iterations);

	RandIter G(F);
	NonZeroRandIter Gn(G);

	bool ret = true;

	for (int k=0;k<iterations;++k) {

		commentator().progress(k);


		Element * A = new Element[m*n];
		Element * B = new Element[m*m];
		Element * C = new Element[m*n];


		// Create B a random matrix of rank n/2
		for (size_t j=0;j<m;++j)
			if ( j % 2 ){
				for (size_t i=0;i<j;++i)
					F.assign (*(B+i*m+j),F.zero);
				for (size_t i=j;i<m;++i)
					Gn.random (*(B+i*m+j));
			}
			else
				for (size_t i=0;i<m;++i)
					F.assign (*(B+i*m+j), F.zero);
		// Create C a random matrix of rank n/2
		for (size_t i = 0; i < m; ++i)
			if ( i % 2 ){
				for (size_t j = 0; j < i; ++j)
					F.assign (*(C+i*n+j),F.zero);
				for (size_t j = i; j < n; ++j)
					Gn.random (*(C+i*n+j));
			}
			else
				for (size_t j = 0; j < n; ++j)
					F.assign (*(C+i*n+j),F.zero);

		// A = B*C
		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, m, n, m,
			      F.one, B, m, C, n, F.zero, A, n );
		delete[] B;
		delete[] C;

		Element * Abis = new Element[m*n];
		for (size_t i=0; i<m*n; ++i)
			*(Abis+i) = *(A+i);

		size_t * P = new size_t[n];
		size_t * Q = new size_t[m];

// 		write_field (F, cerr<<"A="<<endl, A, m, n, n);
		size_t r = FFPACK::LUdivine( F, FFLAS::FflasNonUnit, FFLAS::FflasNoTrans,
					     m, n, A, n, P, Q);
					     //m, n, A, n, P, Q, FFPACK::FfpackLQUP);
// 		write_field (F, cerr<<"LQUP(A)="<<endl, A, m, n, n);

		Element * L = new Element[m*m];
		Element * U = new Element[m*n];

		for (size_t i=0; i<m; ++i){
			for (size_t j = 0; j < i; ++j)
				if (j<n)
					F.assign (*(L+i*m+j), *(A+i*n+j) );
				else
					F.assign (*(L+i*m+j), F.zero );

			for (size_t j = i; j < m; ++j)
				F.assign (*(L+i*m+j), F.zero );
		}
		FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasNoTrans,
				m, 0,(int) r, L, m, Q);
		for (size_t i=0; i<m; ++i)
			F.assign( *(L+i*m+i), F.one);
		for (size_t i=0; i<m; ++i){
			for (size_t j=0; j<i; ++j)
				if (j<n)
					F.assign( *(U+i*n+j), F.zero );
			for (size_t j=i; j<n; ++j)
				F.assign( *(U+i*n+j), *(A+i*n+j) );
		}
// 		write_field (F, cerr<<"L="<<endl, L, m, m, m);
// 		write_field (F, cerr<<"U"<<endl, U, m, n, n);
		// C = U*P
		FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasNoTrans, m,
				  0, (int) r, U, n, P);
		//		write_field (F, cerr<<"UP"<<endl, U, m, n, n);
		// C = Q*C
		FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasTrans, n,
				  0, (int) r, U, n, Q);
		//		write_field (F, cerr<<"QUP"<<endl, U, m, n, n);

		delete[] P;
		delete[] Q;
		// A = L*C
		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, m, n, m,
			      F.one, L, m, U, n, F.zero, A, n );
		delete[] L;
		delete[] U;

		for (size_t i = 0; i < m;++i)
			for (size_t j = 0; j < n; ++j)
				if (!F.areEqual( *(A+i*n+j), *(Abis+i*n+j))){
					ret=false;
				}
		delete[] A;
		delete[] Abis;
		if (!ret){
// 			write_field(F,std::cerr<<"A="<<endl,A,m,n,n);
// 			write_field(F,std::cerr<<"Abis="<<endl,Abis,m,n,n);
// 			write_field(F,std::cerr<<"B="<<endl,B,m,n,n);
// 			write_field(F,std::cerr<<"C="<<endl,C,m,n,n);
		}


	}

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testLQUP");

	return ret;
}

template <class Field>
static bool testMinPoly (const Field& F, size_t n, int iterations)
{
	typedef typename Field::Element                  Element;
	typedef typename Field::RandIter                RandIter;
	typedef typename Field::NonZeroRandIter                NonZeroRandIter;
	typedef typename Givaro::Poly1Dom<Field> PolRing;
	typedef typename PolRing::Element Polynomial;
	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing minpoly"),"testMinPoly",(unsigned int)iterations);
	Element tmp ;
	RandIter G(F);
	NonZeroRandIter Gn(G);
	bool ret = true;

	for (int k=0;k<iterations;++k) {

		commentator().progress(k);

		Element * A = new Element[n*n];
// 		Element * X = new Element[n*(n+1)];
// 		size_t * Perm = new size_t[n];

		Polynomial P;
		// Test MinPoly(In) = X-1
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<n;++j)
				F.assign(*(A+j+i*n),F.zero);
			F.assign(*(A+i*(n+1)),F.one);
		}

		FFPACK::MinPoly(  F, P, n, A, n);

		if ( P.size() !=2 )
			ret = false;
		if ( !F.areEqual(P[0], F.mOne) )
			ret = false;
		if ( !F.areEqual(P[1], F.one) )
			ret = false;
		if(!ret) cerr<<"MinP(In)!=X-1"<<endl;
		// Test MinPoly(a*In) = X-a
		G.random(tmp);

		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<n;++j)
				F.assign(*(A+j+i*n),F.zero);

			F.assign(*(A+i*(n+1)),tmp);
		}

		F.negin(tmp);

		FFPACK::MinPoly( F, P, n, A, n);

		if ( P.size() !=2 )
			ret = false;
		if ( !F.areEqual(P[0], tmp) ){
			cerr<<"P[0]="<<P[0]<<"tmp="<<tmp<<endl;
			ret = false;
		}
		if ( !F.areEqual(P[1], F.one) ){
			cerr<<"P[1]="<<P[1]<<endl;

			ret = false;
		}
		if(!ret) cerr<<"MinP(aIn)!=X-a"<<endl;

#if 1 /*  ??? */
		for (size_t i=0;i<n-1;++i){
			for (size_t j=0; j<n; ++j)
				F.assign(*(A+i*n+j),F.zero);
			F.assign(*(A+i*n+i+1),F.one);
		}
		for (size_t j=0;j<n;++j)
			F.assign(*(A+(n-1)*n+j),F.zero);

		FFPACK::MinPoly( F, P, n, A, n);

		if ( P.size() !=n+1 )
			ret = false;
		for (size_t i=0; i<n;++i)
			if ( !F.areEqual(P[i], F.zero) )
				ret = false;
		if ( !F.areEqual(P[n], F.one) )
			ret = false;
		if(!ret) cerr<<"MinP(J)!=X^n"<<endl;
#endif
		delete[] A;
	}


	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testMinPoly");

	return ret;
}

template <class Field>
static bool testCharPoly (const Field& F, size_t n, int iterations)
{
	typedef typename Field::Element                  Element;
	typedef typename Field::RandIter                RandIter;
	typedef typename Field::NonZeroRandIter         NonZeroRandIter;
	typedef typename Givaro::Poly1Dom<Field> PolRing;
	typedef typename PolRing::Element Polynomial;
	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing charpoly"),"testCharPoly",(unsigned int)iterations);
	Element tmp;
	RandIter G(F);
	NonZeroRandIter Gn(G);
	PolRing R(F);
	bool ret = true;

	for (int k=0;k<iterations;++k) {

		commentator().progress(k);

        typename Field::Element_ptr A = FFLAS::fflas_new(F, n,n);

		std::list<Polynomial> P;
		// Test CharPoly(In) = (X-1)^n
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				F.assign(*(A+i*n+j),F.zero);
			F.assign(*(A+i*(n+1)),F.one);
			for (size_t j=i+1;j<n;++j)
				 F.assign(*(A+i*n+j),F.zero);
		}
		P.clear();

		FFPACK::CharPoly (R, P, n, A, n, G);


		typename list<Polynomial>::const_iterator P_it = P.begin();
		while (P_it != P.end()){
			if ( P_it->size() !=2 ){
				ret = false;
			}
			if ( !F.areEqual(P_it->operator[](0), F.mOne) ){
				ret = false;
			}
			if ( !F.areEqual(P_it->operator[](1), F.one) ){
				ret = false;
			}

			++P_it;
		}

		// Test CharPoly(a*In) = X-a
		G.random(tmp);

		for (size_t i=0;i<n;++i)
			F.assign(*(A+i*(n+1)),tmp);
		F.negin(tmp);
		P.clear();

		FFPACK::CharPoly( R, P, n, A, n,G );

		P_it = P.begin();

		while (P_it != P.end()){
			if ( P_it->size() !=2 )
				ret = false;
			if ( !F.areEqual(P_it->operator[](0), tmp) )
				ret = false;
			if ( !F.areEqual(P_it->operator[](1), F.one) )
			ret = false;
			++P_it;
		}
        FFLAS::fflas_delete (A);
	}

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testCharPoly");

	return ret;
}

template <class Field>
static bool testInv (const Field& F,size_t n, int iterations)
{

	typedef typename Field::RandIter RandIter;
	typedef typename Field::NonZeroRandIter NonZeroRandIter;

	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing inverse"),"testInv",(unsigned int)iterations);

	RandIter G(F);
	NonZeroRandIter Gn(G);

	bool ret = true;

    typename Field::Element_ptr Id = FFLAS::fflas_new(F, n,n);
	for (size_t i=0;i<n;++i){
		for (size_t j=0;j<n;++j)
			F.assign(*(Id+j+i*n),F.zero);
		F.assign( *(Id+i*(n+1)),F.one);
	}
	for (int k=0;k<iterations;++k) {

		commentator().progress(k);


        typename Field::Element_ptr A = FFLAS::fflas_new(F, n,n);
        typename Field::Element_ptr L = FFLAS::fflas_new(F, n,n);
        typename Field::Element_ptr S = FFLAS::fflas_new(F, n,n);


		// create S as an upper triangular matrix of full rank
		// with nonzero random diagonal's element
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				F.assign(*(S+i*n+j),F.zero);
			Gn.random(*(S+i*(n+1)));
			for (size_t j=i+1;j<n;++j)
				G.random(*(S+i*n+j));
		}

		// create L as a lower triangular matrix
		// with only 1's on diagonal
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<i;++j)
				G.random(*(L+i*n+j));
			F.assign(*(L+i*(n+1)),F.one);
			for (size_t j=i+1;j<n;++j)
				F.assign(*(L+i*n+j),F.zero);

		}

		//  compute A=LS

		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, n, n, n,
			      F.one, L, n, S, n, F.zero, A, n );

        typename Field::Element_ptr Ab = FFLAS::fflas_new(F, n,n);
        typename Field::Element_ptr invA = FFLAS::fflas_new(F, n,n);

		for (size_t i = 0; i<n*n; ++i)
			F.assign( *(Ab+i), *(A+i) );
		// compute the inverse of A
		int nullity;
		FFPACK::Invert2 ( F, n, A, n, invA, n, nullity);

		// compute Ainv*A and A*Ainv

		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, n, n, n,
			      F.one, Ab, n, invA, n, F.zero, L, n );

		FFLAS::fgemm( F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, n, n, n,
			      F.one, invA, n, Ab, n, F.zero, S, n );

		for (size_t i=0;i<n*n;++i)
			if ( !F.areEqual(*(L+i),*(Id+i)) || !F.areEqual(*(S+i),*(Id+i)) )
				ret =false;
		if (!ret){
			// write_field (F, cerr<<"ID1="<<endl, L, n,n,n);
// 			write_field (F, cerr<<"ID2="<<endl, S, n,n,n);

		}
        FFLAS::fflas_delete (L);
        FFLAS::fflas_delete (S);
        FFLAS::fflas_delete (A);
        FFLAS::fflas_delete (Ab);
        FFLAS::fflas_delete (invA);
	}
    FFLAS::fflas_delete (Id);

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testInv");

	return ret;
}

template <class Field>
static bool testapplyP (const Field& F,size_t n, int iterations)
{

	typedef typename Field::Element Element;
	typedef typename Field::RandIter RandIter;

	//Commentator commentator;
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start (pretty("Testing applyP"),"testapplyP",(unsigned int)iterations);

	RandIter G(F);
	Element tmp;

	bool ret = true;
	Givaro::Modular<double> Z2(2);
	typename Givaro::Modular<double>::RandIter G2(Z2);
	Givaro::Modular<double>::Element tmp2;

	for (int k=0;k<iterations;++k) {

		commentator().progress(k);


        typename Field::Element_ptr A = FFLAS::fflas_new(F, n,n);
        typename Field::Element_ptr Ab = FFLAS::fflas_new(F, n,n);
		size_t * P =new size_t[n];


		for (size_t i=0; i<n; ++i){
			G.random(tmp);
			if ( Z2.isZero(G2.random(tmp2) ) )
				P[i] = i + ( (size_t) tmp % (n-i) );
			else
				P[i] = i;
		}

		// create S as an upper triangular matrix of full rank
		// with nonzero random diagonal's element
		for (size_t i=0;i<n;++i){
			for (size_t j=0;j<n;++j)
				G.random(*(A+i*n+j));
		}

		for (size_t i = 0; i<n*n; ++i)
			F.assign( *(Ab+i), *(A+i) );

		//  compute A=LS

		FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasNoTrans,
				n, 0,(int)  n, A, n, P );
		FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasNoTrans,
				n, 0,(int)  n, A, n, P );
		FFPACK::applyP( F, FFLAS::FflasRight, FFLAS::FflasTrans,
				n, 0,(int)  n, A, n, P );
		FFPACK::applyP( F, FFLAS::FflasLeft, FFLAS::FflasTrans,
				n, 0,(int)  n, A, n, P );

		for (size_t i=0;i<n*n;++i)
			if ( !F.areEqual(*(Ab+i),*(A+i)) )
				ret =false;
        FFLAS::fflas_delete (A);
        FFLAS::fflas_delete (Ab);
		delete[] P;
	}

	commentator().stop(MSG_STATUS (ret), (const char *) 0, "testApplyP");

	return ret;
}

int main(int argc, char** argv)
{
	bool pass = true;

	static size_t n = 130+(size_t)(130*drand48());
	static size_t m = 130+(size_t)(130*drand48());
	static integer q = 653;
	static int iterations =1;

	static Argument args[] = {
		{ 'n', "-n N", "Set dimension of test matrices to MxN.",       TYPE_INT,     &n },
		{ 'm', "-m M", "Set dimension of test matrices to MxN.",       TYPE_INT,     &m },
		{ 'q', "-q Q", "Operate over the \"field\" GF(Q) [1].", TYPE_INTEGER, &q },
		{ 'i', "-i I", "Perform each test for I iterations.",           TYPE_INT,     &iterations },
		END_OF_ARGUMENTS
	};

	parseArguments (argc, argv, args);


	std::ostream &report = LinBox::commentator().report (LinBox::Commentator::LEVEL_IMPORTANT, INTERNAL_DESCRIPTION);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDepth (3);
	commentator().getMessageClass (INTERNAL_DESCRIPTION).setMaxDetailLevel (Commentator::LEVEL_NORMAL);
	commentator().start("ffpack test suite", "ffpack");


	/* Givaro::Modular Double */
	{
		typedef Givaro::Modular<double> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;

		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);

		pass &= locpass ;
	}

#ifdef _LB_FULL_TEST
	/* Givaro::Modular Float */
	{
		typedef Givaro::Modular<float> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;

		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}

	/* Givaro::Modular Balanced Double */
	{
		typedef Givaro::ModularBalanced<double> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;

		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}

	/* Givaro::Modular Balanced Float */
	{
		typedef Givaro::ModularBalanced<float> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;

		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}
	/* Givaro::Modular int32_t */
	{
		typedef Givaro::Modular<int32_t> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;

		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;

	}

	/* Givaro::Modular Balanced int32_t */
	{
		typedef Givaro::ModularBalanced<int32_t > Field ;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))       locpass = false;
		if (!testDet (F, n, iterations))        locpass = false;
		if (!testapplyP  (F, n, iterations))    locpass = false;
		if (!testInv  (F, n, iterations))       locpass = false;
		if (!testMinPoly (F,n, iterations))      locpass = false;
		if (!testCharPoly (F,n, iterations))     locpass = false;
		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}

	/* Givaro::Modular uint32_t */
	{
		typedef Givaro::Modular<uint32_t> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))   locpass     = false;
		if (!testDet (F, n, iterations))   locpass      = false;
		if (!testapplyP  (F, n, iterations)) locpass    = false;
		if (!testInv  (F, n, iterations)) locpass       = false;
		if (!testMinPoly (F,n, iterations)) locpass      = false;
		if (!testCharPoly (F,n, iterations)) locpass     = false;
		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}

	/* Givaro::Modular int32_t */
#if 0
	{
		typedef Givaro::Modular<int32_t> Field;

		Field F (q);
		F.write(report << "Field : " ) << std::endl;

		srand((unsigned)time (NULL));

		bool locpass = true ;

		if (!testLUdivine (F, m,n, iterations)) locpass = false;
		if (!testRank (F, n, iterations))   locpass     = false;
		if (!testDet (F, n, iterations))   locpass      = false;
		if (!testapplyP  (F, n, iterations)) locpass    = false;
		if (!testInv  (F, n, iterations)) locpass       = false;
		if (!testMinPoly (F,n, iterations)) locpass      = false;
		if (!testCharPoly (F,n, iterations)) locpass     = false;
		(!locpass)?(report << "FAIL" << std::endl):(report << "OK"<<std::endl);
		pass &= locpass ;
	}
#endif
#endif
	commentator().stop(MSG_STATUS(pass),"ffpack test suite");

	return pass ? 0 : -1;
}


#undef _LB_FULL_TEST

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
