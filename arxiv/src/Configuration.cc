// C++ includes.
#include <vector>
#include <string>
#include <iostream>
#include <math.h>

// Custom includes.
#include "Configuration.hh"

Configuration::Configuration( std::string ConfigName )
{
  	Name = ConfigName;
}

std::vector<double>& Configuration::GetParameterGrid()
{
	size_t Index(0);
	const size_t NFields(5);
	const double FHigh(1.0);
	if( NDim == 2 )
	{
		int Offset( 8 * CubeN + 4 );
		ParameterGrid.resize( 128 * (2 * CubeN + 1) * (2 * CubeN + 1) );
		for( int i(0); i < 2 * Offset; ++i )
		{
			for( int j(0); j < 2 * Offset; ++j )
			{
				//0.91825, 0.05405
				ParameterGrid[Index++] = CubeC[0] + ( 0.5 * CubeL[0] ) + ( i - Offset ) * CubeL[0]; //0.89325;
				ParameterGrid[Index++] = CubeC[1] + ( 0.5 * CubeL[1] ) + ( j - Offset ) * CubeL[1]; //0.05450;
			}
		}
	}
	else if( NDim == 3 )
	{
		int Offset( 8 * CubeN + 4 );
		ParameterGrid.resize( 192 * NFields * (2 * CubeN + 1) * (2 * CubeN + 1) );
		for( size_t f(0); f < NFields; ++f )
		{
			for( int i(0); i < 2 * Offset; ++i )
			{
				for( int j(0); j < 2 * Offset; ++j )
				{
					ParameterGrid[Index++] = CubeC[0] + ( 0.5 * CubeL[0] ) + ( i - Offset ) * CubeL[0];
					ParameterGrid[Index++] = pow( 0.5, FHigh - f * 0.05 - 1 ) * ( CubeC[1] + ( 0.5 * CubeL[1] ) + ( j - Offset ) * CubeL[1] ); 
					ParameterGrid[Index++] = FHigh - f * 0.05;
				}
			}
		}
	}
	else std::cerr << "Unsupported parameter dimensions." << std::endl;

	return ParameterGrid;
}
