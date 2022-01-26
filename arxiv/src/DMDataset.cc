// C++ includes.
#include <vector>
#include <algorithm>

// ROOT includes.
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

// Custom includes.
#include "DMDataset.hh"
#include "EvalCUDA_Birks3D.hh"
#include "G4Input.hh"

void DMDataset::Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s, const bool GenerateSummary )
{
	const size_t NPar(3);
	const size_t PSize( Parameters.size() / NPar );
	const size_t NFields( UniqueFields.size() );

	std::vector< std::vector<double> > QY( NFields, std::vector<double>(PSize, 0) );
	
	ReCUDA_Birks3D::DMBirksWrapper( G4Data->GetG4dE(), G4Data->GetG4dx(),
					UniqueFields, Parameters, QY ); 

	if( GenerateSummary ) {
		std::cout << "Summary generation not supported yet!" << std::endl;
	}
	
	double chi2(10000);

	for( size_t pid(0); pid < PSize; ++pid )
	{
		chi2 = 0;
		for( size_t f(0); f < NFields; ++f )
		{
			for( size_t i(0); i < x.size(); ++i )
			{
				if( y.at(i) == UniqueFields.at(f) && IsQY )
				{
					//if( pid == 32 ) std::cout << QY.at(f).at(pid) << std::endl; //"( " << (1000000 / 23.6) * (QY.at(f).at(pid) / x.at(f)) << ", " << z.at(i) << " )" << std::endl;
					chi2 += std::pow( z.at(i) - (1000000 / 23.6) * (QY.at(f).at(pid) / x.at(f)), 2 ) / std::pow( sz.at(i), 2 );
				}
				else if( y.at(i) == UniqueFields.at(f) && !IsQY )
				{
					//if( pid == 32 ) std::cout << "( " << 51.3 - (1000000 / 23.6) * (QY.at(f).at(pid) / x.at(f)) << ", " << z.at(i) << " )" << std::endl;
					chi2 += std::pow( z.at(i) - (51.3 -  (1000000 / 23.6) * (QY.at(f).at(pid) / x.at(f)) ), 2 ) / std::pow( sz.at(i), 2 );
				}
			}
		}
		Chi2s[pid] = chi2;
	}
}
