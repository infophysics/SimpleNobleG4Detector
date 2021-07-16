// C++ includes.
#include <string>
#include <chrono>
#include <vector>

// ROOT includes.
#include "TF2.h"
#include "TFile.h"

// Custom includes.
#include "NuDataset.hh"
#include "DMDataset.hh"
#include "G4Input.hh"
#include "Configuration.hh"

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

  std::cout << "Loading datasets..." << std::endl;

  // Load the G4 data sources.
  G4Input Muons( "Muons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Muon_10keV.root" );
  G4Input Protons( "Protons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root" );
  G4Input Deuterons( "Deuterons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Deuteron_10keV.root" );
  G4Input DM_113Sn_ceK2_364keV( "113Sn_ceK2_364keV", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/113Sn_ceK2_364keV.root" );

  // Configure the Scalettar 1982 DM dataset.
  std::string Scalettar_113Sn_ceK2_364keV_Data( "/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/Scalettar1982_113Sn_ceK2_364keV_QY.csv");
  DMDataset Scalettar_113Sn_ceK2_364keV( "Scalettar_113Sn_ceK2_364keV", Scalettar_113Sn_ceK2_364keV_Data );
  Scalettar_113Sn_ceK2_364keV.SetQorL( true );
  Scalettar_113Sn_ceK2_364keV.SetG4Input( &DM_113Sn_ceK2_364keV );
  std::cout << "Loaded Scalettar 1982 113Sn_ceK2_364keV data." << std::endl;
  
  // Configure the ICARUS muon dataset.
  std::string ICARUS_Muons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_muons_absolute.csv");
  NuDataset ICARUS_Muons("ICARUS_Muons", ICARUS_Muons_Data );
  ICARUS_Muons.SetNBins(100);
  ICARUS_Muons.SetBinL(0.5);
  ICARUS_Muons.SetG4Input( &Muons );
  std::cout << "Loaded ICARUS 3-Ton muons." << std::endl;

  // Configure the ICARUS proton dataset.
  std::string ICARUS_Protons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_3Ton_protons_absolute.csv");
  NuDataset ICARUS_Protons("ICARUS_Protons", ICARUS_Protons_Data );
  ICARUS_Protons.SetNBins(100);
  ICARUS_Protons.SetBinL(0.5);
  ICARUS_Protons.SetG4Input( &Protons );
  std::cout << "Loaded ICARUS 3-Ton protons." << std::endl;

  // Configure the ICARUS T600 muon dataset.
  std::string T600_Muons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ICARUS_T600_protons.csv");
  NuDataset T600_Muons("T600_Muons", T600_Muons_Data );
  T600_Muons.SetNBins(100);
  T600_Muons.SetBinL(0.5);
  T600_Muons.SetG4Input( &Muons );
  std::cout << "Loaded ICARUS T600 Muons." << std::endl;

  // Configure the ArgoNeuT proton dataset.
  std::string ArgoNeuT_Protons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_protons.csv");
  NuDataset ArgoNeuT_Protons("ArgoNeuT_Protons", ArgoNeuT_Protons_Data );
  ArgoNeuT_Protons.SetNBins(100);
  ArgoNeuT_Protons.SetBinL(0.5);
  ArgoNeuT_Protons.SetG4Input( &Protons );
  std::cout << "Loaded ArgoNeuT Protons (combined)." << std::endl;

  // Configure the ArgoNeuT deuteron dataset.
  std::string ArgoNeuT_Deuterons_Data("/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/ArgoNeuT_deuterons.csv");
  NuDataset ArgoNeuT_Deuterons("ArgoNeuT_Deuterons", ArgoNeuT_Deuterons_Data );
  ArgoNeuT_Deuterons.SetNBins(100);
  ArgoNeuT_Deuterons.SetBinL(0.5);
  ArgoNeuT_Deuterons.SetG4Input( &Deuterons );
  std::cout << "Loaded ArgoNeuT Deuterons (combined)." << std::endl;
  

  std::cout << "Loaded all datasets... Done!" << std::endl;

  Configuration Config( "ArgoNeuT Best" );
  Config.SetCubeDim(2);
  Config.SetCubeLen( { 0.000600, 0.00010 } );
  Config.SetCubeCen( { 0.915500, 0.05260 } );
  Config.SetCubeN( 20 );
  std::vector<double> Parameters = Config.GetParameterGrid();
  std::vector<double> Chi2s( Parameters.size() / 2 );

  TFile *SummaryTree = TFile::Open("RegressionSummary_Jul14.root", "recreate");
  TTree *I3Ton_Protons = new TTree("3Ton_Protons", "3Ton_Protons");
  TTree *I3Ton_Muons = new TTree("3Ton_Muons", "3Ton_Muons");
  TTree *IT600_Muons = new TTree("T600_Muons", "T600_Muons");
  TTree *TArgoNeuT_Protons = new TTree("ArgoNeuT_Protons", "ArgoNeuT_Protons");
  TTree *TArgoNeuT_Deuterons = new TTree("ArgoNeuT_Deuterons", "ArgoNeuT_Deuterons");
  TTree *TScalettar_113Sn_ceK2_364keV = new TTree("Scalettar_113Sn_ceK2_364keV", "Scalettar_113Sn_ceK2_364keV");
  double p[2], chi2;
  I3Ton_Protons->Branch("p0", p+0);
  I3Ton_Protons->Branch("p1", p+1);
  I3Ton_Protons->Branch("chi2", &chi2);
  I3Ton_Muons->Branch("p0", p+0);
  I3Ton_Muons->Branch("p1", p+1);
  I3Ton_Muons->Branch("chi2", &chi2);
  IT600_Muons->Branch("p0", p+0);
  IT600_Muons->Branch("p1", p+1);
  IT600_Muons->Branch("chi2", &chi2);
  TArgoNeuT_Protons->Branch("p0", p+0);
  TArgoNeuT_Protons->Branch("p1", p+1);
  TArgoNeuT_Protons->Branch("chi2", &chi2);
  TArgoNeuT_Deuterons->Branch("p0", p+0);
  TArgoNeuT_Deuterons->Branch("p1", p+1);
  TArgoNeuT_Deuterons->Branch("chi2", &chi2);
  TScalettar_113Sn_ceK2_364keV->Branch("p0", p+0);
  TScalettar_113Sn_ceK2_364keV->Branch("p1", p+1);
  TScalettar_113Sn_ceK2_364keV->Branch("chi2", &chi2);
  
  auto start = std::chrono::high_resolution_clock::now();
  ICARUS_Protons.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    I3Ton_Protons->Fill();
  }
  ICARUS_Muons.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    I3Ton_Muons->Fill();
  }
  T600_Muons.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    IT600_Muons->Fill();
  }
  ArgoNeuT_Protons.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    TArgoNeuT_Protons->Fill();
  }
  ArgoNeuT_Deuterons.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    TArgoNeuT_Deuterons->Fill();
  }
  Scalettar_113Sn_ceK2_364keV.Eval( Parameters, Chi2s, false );
  for( size_t i(0); i < Chi2s.size(); ++i)
  {
    p[0] = Parameters[ 2 * i + 0];
    p[1] = Parameters[ 2 * i + 1];
    chi2 = Chi2s[i];
    TScalettar_113Sn_ceK2_364keV->Fill();
  }
  I3Ton_Protons->Write();
  I3Ton_Muons->Write();
  IT600_Muons->Write();
  TArgoNeuT_Protons->Write();
  TArgoNeuT_Deuterons->Write();
  TScalettar_113Sn_ceK2_364keV->Write();
  SummaryTree->Close();
  delete SummaryTree;
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  std::cout << "Time taken by function call: "
	    << duration.count()/1000000.0 << " seconds" << std::endl;
  

  return 0;
}
