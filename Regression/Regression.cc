// C++ includes.
#include <string>

// ROOT includes.
#include "TF2.h"

// Custom includes.
#include "NuDataset.hh"


int main()
{
  std::string ICARUS_Muons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_muons.csv");
  std::string ICARUS_Muons_G4("/home/mueller/Projects/SimpleNobleG4Detector/build/Muon_10keV.root");
  std::string ICARUS_Protons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_protons.csv");
  std::string ICARUS_Protons_G4("/home/mueller/Projects/SimpleNobleG4Detector/build/Proton_10keV.root");
  NuDataset ICARUS_Muons("ICARUS_Muons", ICARUS_Muons_Data, ICARUS_Muons_G4);
  NuDataset ICARUS_Protons("ICARUS_Protons", ICARUS_Protons_Data, ICARUS_Protons_G4);
  ICARUS_Muons.SetNBins(100);
  ICARUS_Muons.SetBinL(0.5);
  ICARUS_Protons.SetNBins(100);
  ICARUS_Protons.SetBinL(0.5);

  TF2 *BareReco = new TF2( "bare_reco", "([0]*x / (1 + ( [1] / y) * x))", 0, 50); // / (23.6/1000)
  //BareReco->SetParameters( 0.87, (0.1225/1.36)*0.5 );
  // 0.878,0.045

  double pCurrent[2], pBest[2], CurrentChi2, BestChi2(10000);
  std::vector< std::vector<double> > Chi2s(31, std::vector<double>(21));
  std::cout << Chi2s.size() << std::endl;

  for( int i(0); i < 31; ++i)
  {
    std::cout << "Iteration " << i << " with current best Chi^2: " << BestChi2 << "." << std::endl;
    for( int j(0); j < 21; ++j)
    {
      pCurrent[0] = 0.888 + (i-15)*(0.01/15.0);
      pCurrent[1] = 0.0485 + (j-10)*(0.005/15.0);
      BareReco->SetParameters( pCurrent[0], pCurrent[1] );
      CurrentChi2 = ICARUS_Muons.Eval( BareReco ) + ICARUS_Protons.Eval( BareReco );
      if( CurrentChi2 < BestChi2 )
      {
	pBest[0] = pCurrent[0];
	pBest[1] = pCurrent[1];
	BestChi2 = CurrentChi2;
      }
      Chi2s.at(i).at(j) = CurrentChi2;
    }
  }
  for( const auto& v: Chi2s )
  {
    for( const auto& s: v ) std::cout << s << ",";
    std::cout << std::endl;
  }
  std::cout << std::endl << "Best Parameters: (" << pBest[0] << ", " << pBest[1] << ") with Chi^2: " << BestChi2 << "." << std::endl;
  BareReco->SetParameters( pBest[0], pBest[1]);
  ICARUS_Muons.Eval( BareReco, true );
  ICARUS_Protons.Eval( BareReco, true);
  
  return 0;
}
