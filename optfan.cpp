#include <stdio.h>
#include "bitefan.h"

#if !defined( sqr )
	#define sqr( x ) (( x ) * ( x ))
#endif // !defined( sqr )

#if !defined( M_PI )
	#define M_PI 3.14159265358979324
#endif // !defined( M_PI )

const int ParamCount = 2;
const int FnCount = 9;
const double CostThreshold = 0.00001;
const int IterCount = 10000;
const int InnerIterCount = 10000;
const int FanParamCount = 9;
const int FanIterCount = 2000;
CBEORnd rnd;

static double roundp( const double x )
{
	return( floor( x * 1000000.0 + 0.5 ) / 1000000.0 );
}

/**
 * Optimization test class.
 */

class CTestOpt : public CBEOOptimizerFan< ParamCount >
{
public:
	int fn;

	virtual void getMinValues( double* const p ) const
	{
		if( fn == 9 )
		{
			p[ 0 ] = -15;
			p[ 1 ] = -3;
		}
		else
		{
			p[ 0 ] = -10 + rnd.getRndValue() * 6;
			p[ 1 ] = -10 + rnd.getRndValue() * 6;
		}
	}

	virtual void getMaxValues( double* const p ) const
	{
		if( fn == 9 )
		{
			p[ 0 ] = -5;
			p[ 1 ] = 3;
		}
		else
		{
			p[ 0 ] = 10 - rnd.getRndValue() * 6;
			p[ 1 ] = 10 - rnd.getRndValue() * 6;
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
		if( fn == 5 ) return( 0.5 + ( sqr( sin( x * x - y * y )) - 0.5 ) / sqr( 1 + 0.001 * ( x * x + y * y )));
		if( fn == 6 ) return( -20 * exp( -0.2 * sqrt( 0.5 * ( x * x + y * y ))) - exp( 0.5 * ( cos( 2 * M_PI * x ) + cos( 2 * M_PI * y ))) + 2.71828182845904524 + 20 );
		if( fn == 7 ) return( 100 * sqr( y - x * x ) + sqr( x - 1 ));
		if( fn == 8 ) return( sqr( 1.5 - x + x * y ) + sqr( 2.25 - x + x * y * y ) + sqr( 2.625 - x + x * y * y * y ));
		if( fn == 9 ) return( 100 * sqrt( fabs( y - 0.01 * x * x )) + 0.01 * fabs( x + 10 ));
		return( 2 * x * x - 1.05 * sqr( sqr( x )) + sqr( sqr( sqr( x ))) / 6 + x * y + y * y );
	}
};

class CFanOpt : public CBEOOptimizerFan< FanParamCount, 3 >
{
public:
	virtual void getMinValues( double* const p ) const
	{
		p[ 0 ] = 0.1;
		p[ 1 ] = 0.1;
		p[ 2 ] = 1.0;
		p[ 3 ] = 0.5;
		p[ 4 ] = 0.1;
		p[ 5 ] = 0.25;
		p[ 6 ] = 0.25;
		p[ 7 ] = 0.25;
		p[ 8 ] = 0.25;
	}

	virtual void getMaxValues( double* const p ) const
	{
		p[ 0 ] = 0.7;
		p[ 1 ] = 0.7;
		p[ 2 ] = 20.0;
		p[ 3 ] = 6.0;
		p[ 4 ] = 0.7;
		p[ 5 ] = 1.1;
		p[ 6 ] = 1.1;
		p[ 7 ] = 1.1;
		p[ 8 ] = 1.1;
	}

	virtual double optcost( const double* const p ) const
	{
		rnd.init( 0 );

		CTestOpt opt;
		opt.CrossProb = roundp( p[ 0 ]);
		opt.CentProb = roundp( p[ 1 ]);
		opt.CentTime = roundp( p[ 2 ]);
		opt.AvgCostMult = roundp( p[ 3 ]);
		opt.AvgDistMult = roundp( p[ 4 ]);
		opt.CrossMults[ 0 ] = roundp( p[ 5 ]);
		opt.CrossMults[ 1 ] = roundp( p[ 6 ]);
		opt.CrossMults[ 2 ] = roundp( p[ 7 ]);
		opt.CentMult = roundp( p[ 8 ]);

		double ItAvg = 0.0;
		double RMSAvg = 0.0;
		double ItRtAvg = 0.0;
		double RjAvg = 0.0;
		int k;

		for( k = 0; k < FnCount; k++ )
		{
			opt.fn = k;
			int Iters[ IterCount ];
			double AvgIter = 0.0;
			double IterAvgCost = 0.0;
			double AvgP1 = 0.0;
			double AvgP2 = 0.0;
			int Rej = 0;
			int j;

			for( j = 0; j < IterCount; j++ )
			{
				int i;

				opt.init( rnd );
				opt.optimize( rnd );

				double PrevBestCost = opt.getBestCost();
				int PrevBestCostCount = 0;

				for( i = 0; i < InnerIterCount; i++ )
				{
					if( opt.getBestCost() < CostThreshold )
					{
						break;
					}

					if( PrevBestCost > opt.getBestCost() )
					{
						PrevBestCost = opt.getBestCost();
						PrevBestCostCount = 0;
					}
					else
					{
						PrevBestCostCount++;

						if( PrevBestCostCount == 10000 )
						{
							i = InnerIterCount;
							break;
						}
					}

					opt.optimize( rnd );
				}

				if( i == InnerIterCount )
				{
					Rej++;
				}

				IterAvgCost += opt.getBestCost();
				AvgP1 += opt.getBestParams()[ 0 ];
				AvgP2 += opt.getBestParams()[ 1 ];

				Iters[ j ] = i;
				AvgIter += i;
			}

			IterAvgCost /= IterCount;
			AvgP1 /= IterCount;
			AvgP2 /= IterCount;

			const double Avg = AvgIter / IterCount;
			double RMS = 0.0;

			for( j = 0; j < IterCount; j++ )
			{
				const double v = Iters[ j ] - Avg;
				RMS += v * v;
			}

			RMS = sqrt( RMS / IterCount );

			ItAvg += Avg;
			RMSAvg += RMS;
			RjAvg += Rej;
			ItRtAvg += RMS / Avg;
		}

		ItAvg /= FnCount;
		RMSAvg /= FnCount;
		ItRtAvg /= FnCount;
		RjAvg /= FnCount;

		return( ItAvg * sqrt( ItRtAvg ));
	}
};

int main()
{
	CBEORnd rnd2;
	rnd2.init( 0 );

	double Params[ FanParamCount ];
	Params[ 0 ] = 0.474072;
	Params[ 1 ] = 0.336040;
	Params[ 2 ] = 8.863409;
	Params[ 3 ] = 1.708017;
	Params[ 4 ] = 0.338186;
	Params[ 5 ] = 0.731545;
	Params[ 6 ] = 0.917677;
	Params[ 7 ] = 0.578602;
	Params[ 8 ] = 0.942713;

	CFanOpt opt;
	opt.init( rnd2, Params );
	int i;

	for( i = 0; i < FanIterCount; i++ )
	{
		opt.optimize( rnd2 );

		printf( "%f\n", opt.getBestCost() );
		int j;

		for( j = 0; j < FanParamCount; j++ )
		{
			Params[ j ] = roundp( opt.getBestParams()[ j ]);
		}

		printf( "CrossProb = %.6f;\n", Params[ 0 ]);
		printf( "CentProb = %.6f;\n", Params[ 1 ]);
		printf( "CentTime = %.6f;\n", Params[ 2 ]);
		printf( "AvgCostMult = %.6f;\n", Params[ 3 ]);
		printf( "AvgDistMult = %.6f;\n", Params[ 4 ]);
		printf( "CrossMults[ 0 ] = %.6f;\n", Params[ 5 ]);
		printf( "CrossMults[ 1 ] = %.6f;\n", Params[ 6 ]);
		printf( "CrossMults[ 2 ] = %.6f;\n", Params[ 7 ]);
		printf( "CentMult = %.6f;\n", Params[ 8 ]);
	}

	return( 0 );
}
