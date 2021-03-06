/* linbox/algorithms/mg-block-lanczos.h
 * Copyright (C) 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <bghovinen@math.waterloo.ca>
 *
 * --------------------------------------------
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ========LICENCE========

 * Class definitions for block Lanczos iteration
 */

#ifndef __LINBOX_mg_block_lanczos_H
#define __LINBOX_mg_block_lanczos_H

#include "linbox/linbox-config.h"
#undef _matT

#include <vector>

#include "linbox/field/archetype.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/blackbox/archetype.h"
#include "linbox/solutions/methods.h"
#include "linbox/matrix/dense-matrix.h"

// I'm putting everything inside the LinBox namespace so that I can drop all of
// this in to LinBox easily at a later date, without any messy porting.

namespace LinBox
{


	/** \brief Block Lanczos iteration
	 *
	 * This is a blocked version of the iteration given in @ref LanczosSolver. The
	 * essential difference is that, rather than applying the black box $A$ to a
	 * single vector \f$v\f$ during each iteration, the block box \f$A\f$ is applied to an
	 * \f$n\times N\f$ matrix \f$V\f$ or, equivalently, to $N$ vectors
	 * \f$v_1, \ldots, v_N\f$ Scalars in the original iteration become \f$N\times N\f$
	 * matrices in the blocked version. The resulting iteration is a natural
	 * extension of the basic theory of the original Lanczos iteration,
	 * c.f. (Montgomery 1995). This has the advantage of more flexible
	 * parallelization, and does not break down as often when used over small
	 * fields.
	 *
	 * Currently, only dense vectors are supported for this iteration, and it is
	 * unlikely any other vector archetypes will be supported in the future.
	 */
	template <class Field, class Matrix = BlasMatrix<Field,typename RawVector<typename Field::Element >::Dense> >
	class MGBlockLanczosSolver {
	public:

		typedef typename Field::Element Element;

		/** Constructor
		 * @param F Field over which to operate
		 * @param traits @ref SolverTraits  structure describing user
		 *               options for the solver
		 */
		MGBlockLanczosSolver (const Field &F, const BlockLanczosTraits &traits) :
			_traits (traits), _field (&F), _VD (F), _MD (F), _randiter (F)
			,_AV(F)
			, _block (traits.blockingFactor ())
		{
			init_temps ();
		}

		/** Constructor with a random iterator
		 * @param F Field over which to operate
		 * @param traits @ref SolverTraits  structure describing user
		 *               options for the solver
		 * @param r Random iterator to use for randomization
		 */
		MGBlockLanczosSolver (const Field &F, const BlockLanczosTraits &traits, typename Field::RandIter r) :
			_traits (traits), _field (&F), _VD (F), _MD (F), _randiter (r)
			,_AV(F)
			, _block (traits.blockingFactor ())
		{
			init_temps ();
		}

		/** Solve the linear system Ax = b.
		 *
		 * If the system is nonsingular, this method computes the unique
		 * solution to the system Ax = b. If the system is singular, it computes
		 * a random solution.
		 *
		 * If the matrix A is nonsymmetric, this method preconditions the matrix
		 * A with the preconditioner D_1 A^T D_2 A D_1, where D_1 and D_2 are
		 * random nonsingular diagonal matrices. If the matrix A is symmetric,
		 * this method preconditions the system with A D, where D is a random
		 * diagonal matrix.
		 *
		 * @param A Black box for the matrix A
		 * @param x Vector in which to store solution
		 * @param b Right-hand side of system
		 * @return true on success and false on failure
		 */
		template <class Blackbox, class Vector>
		bool solve (const Blackbox &A, Vector &x, const Vector &b);

		/** Sample uniformly from the (right) nullspace of A
		 *
		 * @param A Black box for the matrix A
		 * @param x Matrix into whose columns to store nullspace elements
		 * @return Number of nullspace vectors found
		 */
		template <class Blackbox, class Matrix1>
		unsigned int sampleNullspace (const Blackbox &A, Matrix1 &x);

	private:

		// S_i is represented here as a vector of booleans, where the entry at
		// index j is true if and only if the corresponding column of V_i is to
		// be included in W_i

		// All references to Winv are actually -Winv

		// Run the block Lanczos iteration and return the result. Return false
		// if the method breaks down. Do not check that Ax = b in the end
		template <class Blackbox>
		bool iterate (const Blackbox &A);

		// Compute W_i^inv and S_i given V_i^T A V_i
		int compute_Winv_S (Matrix            &Winv,
				    std::vector<bool> &S,
				    const Matrix      &T);

		// Given B with N columns and S_i, compute B S_i S_i^T
		template <class Matrix1, class Matrix2>
		Matrix1 &mul_SST (Matrix1                 &BSST,
				  const Matrix2           &B,
				  const std::vector<bool> &S) const;

		// Matrix-matrix multiply
		// C = A * B * S_i * S_i^T
		template <class Matrix1, class Matrix2, class Matrix3>
		Matrix1 &mul (Matrix1                 &C,
			      const Matrix2           &A,
			      const Matrix3           &B,
			      const std::vector<bool> &S) const;

		// In-place matrix-matrix multiply on the right
		// A = A * B * S_i * S_i^T
		// This is a version of the above optimized to use as little additional
		// memory as possible
		template <class Matrix1, class Matrix2>
		Matrix1 &mulin (Matrix1                 &A,
				const Matrix2           &B,
				const std::vector<bool> &S) const;

		// Matrix-vector multiply
		// w = A * S_i * S_i^T * v
		template <class Vector1, class Matrix1, class Vector2>
		Vector1 &vectorMul (Vector1                 &w,
				    const Matrix1           &A,
				    const Vector2           &v,
				    const std::vector<bool> &S) const;

		// Matrix-vector transpose multiply
		// w = (A * S_i * S_i^T)^T * v
		template <class Vector1, class Matrix1, class Vector2>
		Vector1 &vectorMulTranspose (Vector1                 &w,
					     const Matrix1           &A,
					     const Vector2           &v,
					     const std::vector<bool> &S) const;

		// Matrix-matrix addition
		// A = A + B * S_i * S_i^T
		template <class Matrix1, class Matrix2>
		Matrix1 &addin (Matrix1                 &A,
				const Matrix2           &B,
				const std::vector<bool> &S) const;

		// Add I_N to the given N x N matrix
		// A = A + I_N
		template <class Matrix1>
		Matrix1 &addIN (Matrix1 &A) const;

		// Given a vector S of bools, write an array of array indices in which
		// the true values of S are last
		void permute (std::vector<size_t>     &indices,
			      const std::vector<bool> &S) const;

		// Set the given matrix to the identity
		template <class Matrix1>
		Matrix1 &setIN (Matrix1 &A) const;

		// Find a suitable pivot row for a column and exchange it with the given
		// row
		bool find_pivot_row (Matrix                    &A,
				     size_t                     row,
				     int                        col_offset,
				     const std::vector<size_t> &indices);

		// Eliminate all entries in a column except the pivot row, using row
		// operations from the pivot row
		void eliminate_col (Matrix                    &A,
				    size_t                     pivot,
				    int                        col_offset,
				    const std::vector<size_t> &indices,
				    const Element             &Ajj_inv);

		// Initialize the temporaries used in computation
		void init_temps ();

		// are we public?
		inline const Field & field() const { return *_field; }

		// Private variables

		const BlockLanczosTraits _traits;
		const Field              *_field;
		VectorDomain<Field>       _VD;
		MatrixDomain<Field>       _MD;
		typename Field::RandIter  _randiter;

		// Temporaries used in the computation

		// Matrix  _matV[3];             // n x N
		std::vector<Matrix> _matV;
		Matrix  _AV;               // n x N
		Matrix  _VTAV;             // N x N
		// Matrix  _Winv[2];          // N x N
		std::vector<Matrix> _Winv ;
		Matrix  _AVTAVSST_VTAV;    // N x N
		Matrix  _matT;                // N x N
		Matrix  _DEF;              // N x N
		std::vector<bool>         _vecS;                // N-vector of bools

		Matrix _x;                 // n x <=N
		Matrix _y;                 // n x <=N
		Matrix _b;                 // n x <=N

		mutable Matrix _tmp;       // N x <=N
		mutable Matrix _tmp1;      // N x <=N


		std::vector<size_t>       _indices;          // N

		mutable Matrix _matM;         // N x 2N

		// Blocking factor

		size_t                    _block;

		// Construct a transpose matrix on the fly
		template <class Matrix1>
		TransposeMatrix<Matrix1> transpose (Matrix1 &M) const
		{ return TransposeMatrix<Matrix1> (M); }

	protected:

		template <class Matrix1>
		bool isAlmostIdentity (const Matrix1 &M) const;

		// Test suite for the above functions

		bool test_compute_Winv_S_mul (int n) const;
		bool test_compute_Winv_S_mulin (int n) const;
		bool test_mul_SST (int n) const;
		bool test_mul_ABSST (int n) const;
		bool test_mulTranspose (int m, int n) const;
		bool test_mulTranspose_ABSST (int n) const;
		bool test_mulin_ABSST (int n) const;
		bool test_addin_ABSST (int n) const;

	public:

		bool runSelfCheck () const;
	};

} // namespace LinBox

#include "linbox/algorithms/mg-block-lanczos.inl"

#endif // __LINBOX_mg_block_lanczos_H

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
