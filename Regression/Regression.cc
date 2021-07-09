// C++ includes.
#include <string>
#include <chrono>

// ROOT includes.
#include "TF2.h"
#include "TFile.h"

// Custom includes.
#include "NuDataset.hh"


int main()
{
  // Strategy: The underlying "bare" recombination model is applied to the individual Geant4 steps.
  // This allows us to more accurately represent the microphysical environment in which the
  // ionization and recombination occurring (e.g. individual delta rays). The alternative approach
  // (and the prevalent one) is to treat recombination macroscopically by fitting data from tracks
  // and lower energy deposits to an "effective" recombination model.
  
  // First we must configure the underlying bare recombination model using a TF2 ROOT object. The
  // datasets that the model is compared against are abstracted to C++ objects. In this way we are
  // able to tailor the underlying functionality to the individual datasets, while maintaining a
  // uniform way of including each dataset in the regression. For example, underlying the chi^2
  // computation for each dataset is a Geant4 MC file which contains the raw simulation information
  // for that particular source. Each Dataset (the object) knows how to take these MC files and
  // convert them to a predicted model (given a bare recombination model) against which a chi^2
  // score for the dataset can be computed.

  // The general process is to configure the TF2 representing the bare recombination model and each
  // individual Dataset, then perform a grid search of the parameter space. 

  // Configure the "bare" recombination model.
  //TF2 *BareReco = new TF2( "bare_reco", "([0]*x / (1 + ( [1] / y) * x))", 0, 50);
  
  // Configure the ICARUS muon dataset.
  std::string ICARUS_Muons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_muons.csv");
  std::string ICARUS_Muons_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Muon_10keV.root");
  NuDataset ICARUS_Muons("ICARUS_Muons", ICARUS_Muons_Data, ICARUS_Muons_G4);
  ICARUS_Muons.SetNBins(100);
  ICARUS_Muons.SetBinL(0.5);

  // Configure the ICARUS proton dataset.
  std::string ICARUS_Protons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_protons.csv");
  std::string ICARUS_Protons_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root");
  NuDataset ICARUS_Protons("ICARUS_Protons", ICARUS_Protons_Data, ICARUS_Protons_G4);
  ICARUS_Protons.SetNBins(100);
  ICARUS_Protons.SetBinL(0.5);

  // Configure the ArgoNeuT proton datasets.
  std::string ArgoNeuT_Protons_80_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_80.csv");
  std::string ArgoNeuT_Protons_80_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root");
  NuDataset ArgoNeuT_Protons_80("ArgoNeuT_Protons_80", ArgoNeuT_Protons_80_Data, ArgoNeuT_Protons_80_G4);
  ArgoNeuT_Protons_80.SetNBins(100);
  ArgoNeuT_Protons_80.SetBinL(0.5);

  std::string ArgoNeuT_Protons_60_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_60.csv");
  std::string ArgoNeuT_Protons_60_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root");
  NuDataset ArgoNeuT_Protons_60("ArgoNeuT_Protons_60", ArgoNeuT_Protons_60_Data, ArgoNeuT_Protons_60_G4);
  ArgoNeuT_Protons_60.SetNBins(100);
  ArgoNeuT_Protons_60.SetBinL(0.5);

  std::string ArgoNeuT_Protons_50_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_50.csv");
  std::string ArgoNeuT_Protons_50_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root");
  NuDataset ArgoNeuT_Protons_50("ArgoNeuT_Protons_50", ArgoNeuT_Protons_50_Data, ArgoNeuT_Protons_50_G4);
  ArgoNeuT_Protons_50.SetNBins(100);
  ArgoNeuT_Protons_50.SetBinL(0.5);

  std::string ArgoNeuT_Protons_40_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons_40.csv");
  std::string ArgoNeuT_Protons_40_G4("/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root");
  NuDataset ArgoNeuT_Protons_40("ArgoNeuT_Protons_40", ArgoNeuT_Protons_40_Data, ArgoNeuT_Protons_40_G4);
  ArgoNeuT_Protons_40.SetNBins(100);
  ArgoNeuT_Protons_40.SetBinL(0.5);

  std::vector<double> Parameters(64);
  for( size_t i(0); i < 32; ++i )
  {
    Parameters[ 2 * i + 0 ] = 0.9155;
    Parameters[ 2 * i + 1 ] = 0.0526;
  }
  std::vector<double> Chi2s(32, 0);

  auto start = std::chrono::high_resolution_clock::now();
  ICARUS_Protons.Eval( Parameters, Chi2s );
  std::cout << "Resulting chi^2: " << Chi2s[0] << std::endl;
  ICARUS_Muons.Eval( Parameters, Chi2s );
  std::cout << "Resulting chi^2: " << Chi2s[0] << std::endl;
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  std::cout << "Time taken by function call: "
	    << duration.count()/1000000.0 << " seconds" << std::endl;
  
  // Configure the variables for the parameter grid search, including a TTree to dump the results of
  // each point in parameter space into for later access.
  //double pCurrent[2], pBest[2], CurrentChi2, BestChi2(10000);
  //double pBest[2];
  /*TFile *RegSummary = TFile::Open("RegressionSummary6.root", "recreate");
  TTree *RegTree = new TTree("RegTree", "RegTree");
  RegTree->Branch("p0", pCurrent+0);
  RegTree->Branch("p1", pCurrent+1);
  RegTree->Branch("chi2", &CurrentChi2);


  double ParN[2] = { 50, 50 };
  double ParW[2] = { 0.000250, 0.00010 };
  double ParC[2] = { 0.912625, 0.05405 }; //{ 0.91250, 0.05400 };
  pBest[0] = ParC[0];
  pBest[1] = ParC[1];
  
  for( int i( -1 * ParN[0] ); i <= ParN[0]; ++i)
  {
    std::cout << "Iteration " << i + ParN[0] << " with current best Chi^2: " << BestChi2 << "." << std::endl;
    for( int j( -1 * ParN[1] ); j <= ParN[1]; ++j)
    {
      pCurrent[0] = ParC[0] + i * ParW[0];
      pCurrent[1] = ParC[1] + j * ParW[1];
      BareReco->SetParameters( pCurrent[0], pCurrent[1] );
      CurrentChi2 = ICARUS_Muons.Eval( BareReco ) + ICARUS_Protons.Eval( BareReco );
      if( CurrentChi2 < BestChi2 )
      {
	pBest[0] = pCurrent[0];
	pBest[1] = pCurrent[1];
	BestChi2 = CurrentChi2;
      }
      RegTree->Fill();
    }
  }

  RegTree->Write();
  RegSummary->Close();
  delete RegSummary;

  std::cout << "Best Parameters: (" << pBest[0] << ", " << pBest[1] << ") with Chi^2: " << BestChi2 << "." << std::endl;*/

  //pBest[0] = 0.915875;
  //pBest[1] = 0.052650;
  //BareReco->SetParameters( pBest[0], pBest[1]);
  
  //auto start = std::chrono::high_resolution_clock::now();

  //for( unsigned int l(0); l < 10; ++l)
  //{
  //ICARUS_Muons.Eval( BareReco, true );
  //ICARUS_Protons.Eval( BareReco, true);
  //ArgoNeuT_Protons_80.Eval( BareReco, true);
  //ArgoNeuT_Protons_60.Eval( BareReco, true);
  //ArgoNeuT_Protons_50.Eval( BareReco, true);
  //ArgoNeuT_Protons_40.Eval( BareReco, true);
    //}
  
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  //std::cout << "Time taken by function call: "
  //    << duration.count()/10000000.0 << " seconds" << std::endl;

  return 0;
}
