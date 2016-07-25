#include <stdio.h>
#include "biteopt.h"
#include "biteopt2.h"
#include "bitehive.h"
//#include "CNMSimplexSolver.h"

#if !defined( sqr )
	#define sqr( x ) (( x ) * ( x ))
#endif // !defined( sqr )

#if !defined( M_PI )
	#define M_PI 3.14159265358979324
#endif // !defined( M_PI )

const int ParamCount = 2;
const int FnCount = 6;

/**
 * Optimization test class.
 */

class CTestOpt : public /*CBEOHive< ParamCount, 1 >*/ CBEOOptimizer2< ParamCount >
{
public:
	int fn;

	virtual void getMinValues( double* const p ) const
	{
		if( fn == 9 )
		{
			p[ 0 ] = -10-0.01;//-15;
			p[ 1 ] = 1-0.01;//-3;
		}
		else
		{
			p[ 0 ] = -10;
			p[ 1 ] = -10;
		}
	}

	virtual void getMaxValues( double* const p ) const
	{
		if( fn == 9 )
		{
			p[ 0 ] = -10+0.01;//-5;
			p[ 1 ] = 1+0.01;//3;
		}
		else
		{
			p[ 0 ] = 10;
			p[ 1 ] = 10;
		}
	}

	virtual double optcost( const double* const p ) const
	{
		const double x = p[ 0 ];
		const double y = p[ 1 ];

		if( fn == 1 ) return( sqr( x + 2 * y - 7 ) + sqr( 2 * x + y - 5 ));
		if( fn == 2 ) return( 0.26 * ( x * x + y * y ) - 0.48 * x * y );
		if( fn == 3 ) return( x * x + y * y );
		if( fn == 4 ) return( sqr( sin( 3 * M_PI * x )) + sqr( x - 1 ) * ( 1 + sqr( sin( 3 * M_PI * y ))) + sqr( y - 1 ) * ( 1 + sqr( sin( 2 * M_PI * y ))));
		if( fn == 5 ) return( sqr( 1.5 - x + x * y ) + sqr( 2.25 - x + x * y * y ) + sqr( 2.625 - x + x * y * y * y ));
		if( fn == 6 ) return( -20 * exp( -0.2 * sqrt( 0.5 * ( x * x + y * y ))) - exp( 0.5 * ( cos( 2 * M_PI * x ) + cos( 2 * M_PI * y ))) + 2.71828 + 20 );
		if( fn == 7 ) return( 0.5 + ( sqr( sin( x * x - y * y )) - 0.5 ) / sqr( 1 + 0.001 * ( x * x + y * y )));
		if( fn == 8 ) return( 100 * sqr( y - x * x ) + sqr( x - 1 ));
		if( fn == 9 ) return( 100 * sqrt( fabs( y - 0.01 * x * x )) + 0.01 * fabs( x + 10 ));
		return( 2 * x * x - 1.05 * sqr( sqr( x )) + sqr( sqr( sqr( x ))) / 6 + x * y + y * y );
	}
};

int main()
{
	CTestOpt opt;
	CBEORnd rnd;
	rnd.init( 0 );

	double FnAvg = 0.0;
	int k;

	for( k = 0; k < FnCount; k++ )
	{
		opt.fn = k;
		const int IterCount = 10000;
		int Iters[ IterCount ];
		int AvgIter = 0;
		double AvgCost = 0.0;
		double AvgP1 = 0.0;
		double AvgP2 = 0.0;
		int j;

		for( j = 0; j < IterCount; j++ )
		{
			double Params[ ParamCount ];
			double minv[ ParamCount ];
			double maxv[ ParamCount ];
			opt.getMinValues( minv );
			opt.getMaxValues( maxv );
			int i;

			for( i = 0; i < ParamCount; i++ )
			{
				Params[ i ] = minv[ i ] + ( maxv[ i ] - minv[ i ]) *
					rnd.getRndValue();
			}

			i = 10000;
/*			AvgCost += vox :: solveNMSimplex( opt, ParamCount, Params, true,
				0.0001, &i );

			AvgP1 += Params[ 0 ];
			AvgP2 += Params[ 1 ];
*/
			opt.init( rnd, Params );

			for( i = 0; i < 10000; i++ )
			{
				opt.optimize( rnd );

				if( opt.getBestCost() < 0.001 )
				{
					break;
				}
			}

			AvgCost += opt.getBestCost();
			AvgP1 += opt.getBestParams()[ 0 ];
			AvgP2 += opt.getBestParams()[ 1 ];

			Iters[ j ] = i;
			AvgIter += i;
		}

		AvgCost /= IterCount;
		AvgP1 /= IterCount;
		AvgP2 /= IterCount;

		double Avg = (double) AvgIter / IterCount;
		double RMS = 0.0;

		for( j = 0; j < IterCount; j++ )
		{
			double v = Iters[ j ] - Avg;
			RMS += v * v;
		}

		RMS = sqrt( RMS / IterCount );

		printf( "AvgIter:%6.1f RMSIter:%6.1f AvgCost:%9.6f %6.3f %6.3f\n", Avg,
			RMS, AvgCost, AvgP1, AvgP2 );

		FnAvg += Avg;
	}

	FnAvg /= FnCount;
	printf( "FnAvg: %.1f\n", FnAvg );

	return( 0 );
}
