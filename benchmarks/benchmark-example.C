/* Copyright (C) 2011 LinBox
 * Written by Brice Boyer (briceboyer) <boyer.brice@gmail.com>
 *
 *
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

/*! @file benchmarks/benchmark-example.C
 * @ingroup benchmarks
 * @brief Benchmarking example
 * @example benchmark
 */

#include "benchmarks/benchmark.h"
#include "linbox/util/error.h"
#include "fflas-ffpack/fflas-ffpack.h"
#include "linbox/ring/modular.h"
#include "linbox/matrix/random-matrix.h"
#include "linbox/matrix/dense-matrix.h"
#include "linbox/matrix/matrix-domain.h"
#include "linbox/matrix/sparse-matrix.h"
#include "linbox/solutions/rank.h"

#include <givaro/modular-balanced.h>


using namespace LinBox ;
using Givaro::Timer;

/* compute MegaFLOPS for mat mul (2 m n k) */
double mm_mflops(size_t m, size_t n, size_t k)
{
	return 2*(double)m/100*(double)n/100*(double)k/100 ;
}

/* Benchmark on a field */

/*! @internal
 * @brief launches the benchmarks for the square case.
 * @param F field
 * @param min min size to bench
 * @param max max size to bench
 * @param step step between two sizes
 * @param Data where data is stored
 * @param series_nb index of the current series of measures.
 */
template<class Field>
void launch_bench_square(Field & F // const problem
			 , size_t min, size_t max, size_t step // no negative step
			 , PlotData & Data
			)
{
	linbox_check(step);
	linbox_check(min <= max);

	std::ostringstream nam ;
	F.write(nam);
	// Data.setCurrentSeriesName(nam.str());

	Data.newSeries(nam.str());
	Chrono<Timer> TW ;

	typedef typename Field::RandIter Randiter ;
	Randiter R(F) ;
	BlasMatrixDomain<Field> BMD(F) ;
	RandomDenseMatrix<Randiter,Field> RandMat(F,R);

	for ( size_t i = min ; i < max ; i += step ) {

		showAdvanceLinear(i,min,max);
		size_t ii = i ;
		BlasMatrix<Field> A (F,ii,ii);
		BlasMatrix<Field> B (F,ii,ii);
		BlasMatrix<Field> C (F,ii,ii);

		size_t j = 0 ; // number of repets.

		RandMat.random(A);
		RandMat.random(B);
		RandMat.random(C);
		TW.clear() ;
		while( Data.keepon(j,TW.time(),false) ) {
			TW.start() ;
			BMD.mul(C,A,B) ; // C = AB
			TW.stop();
			++j ;
		}
		double mflops = computeMFLOPS(TW.times(),mm_mflops(i,i,i));

		Data.setCurrentSeriesEntry(i,mflops,(double)i,TW.time()); // could be i*i*i

	}

	Data.finishSeries();
}

/* Collects Benchmarks */


/*! @brief Benchmark square fgemm Y=AX for several fields.
 * @param min min size
 * @param max max size
 * @param step step of the size between 2 benchmarks
 * @param charac characteristic of the field.
 */
void bench_square( size_t min, size_t max, size_t step, int charac )
{

	size_t nb = 1 ;// une col de plus (la première)
	typedef Givaro::Modular<double>          Field0 ; ++nb ;
	typedef Givaro::Modular<float>           Field1 ; ++nb ;
	// typedef Givaro::Modular<int32_t>         Field2 ; ++nb ;
	// typedef Givaro::ModularBalanced<double>  Field3 ; ++nb ;
	// typedef Givaro::ModularBalanced<float>   Field4 ; ++nb ;
	// typedef Givaro::ModularBalanced<int32_t> Field5 ; ++nb ;
	// GivaroZpZ

	///// DATA HARVEST ////

	PlotData  Data;
	showProgression Show(nb) ;

	Field0 F0(charac) ;
	// launch_bench_square<Field0>(F0,100,1500,300,Data);
	launch_bench_square<Field0>(F0,min,max,step,Data);
	Show.FinishIter();

	if (charac < 2048) {
		Field1 F1(charac) ;
		// launch_bench_square(F1,200,1500,200,Data);
		launch_bench_square(F1,min,max,step,Data);
		Show.FinishIter();
	}
	else {
		Show.SkipIter();
	}



#if 0
	Field2 F2(charac) ;
	launch_bench_square(F2,min,max,step,Data);
	Show.FinishIter();

	Field3 F3(charac) ;
	launch_bench_square(F3,min,max,step,Data);
	Show.FinishIter();

	if (charac < 2048) {
		Field4 F4(charac) ;
		launch_bench_square(F4,min,max,step,Data);
		Show.FinishIter();
	}
	else {
		Show.SkipIter();
	}

	Field5 F5(charac) ;
	launch_bench_square(F5,min,max,step,Data);
	Show.FinishIter();
#endif

	///// PLOT STYLE ////
	LinBox::PlotStyle Style;

	Style.setTerm(LinBox::PlotStyle::Term::eps);
	Style.setTitle("BlasMatrixDomain mul","Mflops","dimensions");

	Style.setPlotType(LinBox::PlotStyle::Plot::graph);
	Style.setLineType(LinBox::PlotStyle::Line::linespoints);


	LinBox::PlotGraph Graph(Data,Style);
	Graph.setOutFilename("bmdmul_square");

	// Graph.plot();

	Graph.print(Tag::Printer::gnuplot);

	Graph.print(Tag::Printer::tex);
	Graph.print(Tag::Printer::csv);

	return ;

}

template<class Field>
void launch_bench_rank(const Field &F, const std::string & name
			 , PlotData & Data
		       )
{

	SparseMatrix<Field> Mat(F);
	std::ifstream mat1 (name);
	Mat.read(mat1);

	MatrixMetaData mmd (Mat,name);

	// Data.newSeries(name);
	Chrono<Timer> TW ;

	showAdvanceLinear(1,1,2);

	TW.clear() ;
	size_t j = 0 ;
	while( Data.keepon(j,TW.time()) ) {
		TW.start() ;
		unsigned long d ;
		LinBox::rank(d,Mat,Method::Blackbox());
		TW.stop();
		++j ;
	}

	// double mflops = computeMFLOPS(TW.times(),mm_mflops(i,i,i));

	Data.setSeriesEntry("Rank (Blackbox)",name,TW.time(),(double)Mat.size(),TW.time());
	Data.addCurrentEntryMetaData(mmd);
	// Data.addCurrentSeriesMetaData(mmd);
	// Data.addMetaData(mmd);


	showAdvanceLinear(2,1,2);

	TW.clear() ;
	j = 0 ;
	while( Data.keepon(j,TW.time()) ) {
		TW.start() ;
		unsigned long d ;
		LinBox::rank(d,Mat,Method::SparseElimination());
		TW.stop();
		++j ;
	}

	Data.selectSeries("Rank (SparseElimination)");
	Data.setCurrentSeriesEntry(name,TW.time(),(double)Mat.size(),TW.time());
	Data.addCurrentEntryMetaData(mmd);

}

void bench_rank(int carac)
{
	typedef Givaro::Modular<double>  Field0 ;
	Field0 F(carac);
	int nb = 1;

	//! @bug no gz reader ?

	std::string m1 = "matrix/bibd_12_5_66x792.sms" ; ++nb;
	std::string m2 = "matrix/bibd_13_6_78x1716.sms" ; ++nb;
	std::string m3 = "matrix/bibd_14_7_91x3432.sms" ; ++nb;

	PlotData  Data;
	showProgression Show((size_t)nb) ;

	launch_bench_rank(F,m1,Data);
	Show.FinishIter();

	launch_bench_rank(F,m2,Data);
	Show.FinishIter();

	launch_bench_rank(F,m3,Data);
	Show.FinishIter();

	///// PLOT STYLE ////
	LinBox::PlotStyle Style;
	Style.setTerm(LinBox::PlotStyle::Term::eps);
	Style.setTitle("Rank algorithms","seconds","matrices");
	Style.setXtics(LinBox::PlotStyle::Options::oblique);

	/* default
	Style.setPlotType(LinBox::PlotStyle::Plot::histo);
	Style.setLineType(LinBox::PlotStyle::Line::histogram);
	*/


	LinBox::PlotGraph Graph(Data,Style);
	Graph.setOutFilename("rank_comparison");

	// Graph.plot();

	Graph.print(Tag::Printer::gnuplot);

	// change style
	Graph.refStyle().setTerm(LinBox::PlotStyle::Term::png);
	Graph.print(Tag::Printer::gnuplot);

	Graph.print(Tag::Printer::xml);
	Graph.print(Tag::Printer::html);

	return;
}

/*  main */

int main( int ac, char ** av)
{
	/*  Argument parsing/setting */

	static size_t       min  = 100;     /*  min size */
	static size_t       max  = 1500;    /*  max size (not included) */
	static size_t       step = 300;    /*  step between 2 sizes */
	// static std::list<int> lst  ;       /*  what bench to start ? */
	// lst.push_front(1);// ={1,2} vivement le nouveau std...
	// lst.push_front(2);

	static Argument as[] = {
		{ 'm', "-m min" , "Set minimal size of matrix to test."    , TYPE_INT , &min },
		{ 'M', "-M Max" , "Set maximal size."                      , TYPE_INT , &max },
		{ 's', "-s step", "Sets the gap between two matrix sizes.", TYPE_INT , &step },
		// { 'l', "-l list", "Only launches a subset of available benchmarks\n - 1: compare to raw blas\n - 2:various square sizes\n - 3:various shapes\n - 4: various parameters (a,b)\n - 5 : various transp. combinations", TYPE_INTLIST, &lst },
		END_OF_ARGUMENTS
	};

	parseArguments (ac, av, as);

	if (min >= max) {
		throw LinBoxError("min value should be smaller than max...");
	}
	if (min + step >= max) {
		std::cout << "Warning : your x axis has only one point. You should have a smaller step." << std::endl;
	}

	/* square for various fields */
#if 1
	{
		std::cout << " *** Lines plot *** " << std::endl;
		std::cout << "Benchmark square matrix multiplication via BMD.mul()" << std::endl;
		bench_square(min,max,step,13);
	}
#endif

	/* different sparse matrix   */

	{
		std::cout << " *** Bar plot *** " << std::endl;
		std::cout << "Benchmark different matrices on different rank algorithms" << std::endl;
		bench_rank(13);
	}



	return EXIT_SUCCESS ;
}

// Local Variables:
// mode: C++
// tab-width: 4
// indent-tabs-mode: nil
// c-basic-offset: 4
// End:
// vim:sts=4:sw=4:ts=4:et:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s
