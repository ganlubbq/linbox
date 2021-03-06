/*
 * examples/blackbox/load-det.C
 *
 * Copyright (C) 2001, 2002, 2010 Bradford Hovinen <hovinen@cis.udel.edu>
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
 */

/** @name examples/blackbox/load-det.C
 *
 * @author Bradford Hovinen <hovinen@cis.udel.edu>
 *
 * @memo
 * Small program that loads and computes the determinant of a matrix whose
 * filename is given on the command line.
 */
//@{
#include "linbox/linbox-config.h"

#include <fstream>
#include <vector>

#include "linbox/field/modular.h"
#include "linbox/solutions/det.h"

using namespace LinBox;
using namespace std;

// Select our field: integers modulo a word-size (max. 31-bit) modulus
typedef Givaro::Modular<uint32_t> Field;

// Select our black box: a sparse matrix over the above-mentioned field with
// default application vector and row representation types
typedef DenseMatrix<Field> Blackbox;

// Constants: we are working with a matrix over GF(q)

/// load-det matrix-file
int main (int argc, char **argv)
{
	Field::Element det_A;
	int q = 65521U;

	if (argc < 2 || argc > 3) {
		cerr << "Usage: load-det <matrix> [<p>]" << endl;
		return -1;
	}

	commentator().setMaxDepth (2);
	commentator().setReportStream (cout);

	if (argc == 3) q = atoi(argv[2]);
	Field F (q);

	Blackbox A (F);
	ifstream input (argv[1]);

	if (!input) {
		cerr << "Error: Cannot load matrix " << argv[1] << endl;
		return -1;
	}

	A.read (input); // size is determined by the input.
	cout << "A is " << A.rowdim() << " by " << A.coldim() << endl;

	det (det_A, A, F);

	cout << "Determinant is " << det_A << " mod " << q << endl;

	return 0;
}
//@}

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
