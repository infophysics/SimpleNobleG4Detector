// C++ includes.
#include <string>
#include <chrono>
#include <map>
#include <iostream>

// ROOT includes.
#include "TFile.h"
#include "TTree.h"

// Custom includes.
#include "RegContainer.hh"
#include "G4Input.hh"
#include "NuDataset.hh"
#include "DMDataset.hh"

RegContainer::RegContainer()
{
  // Settings for the regression (declared as constants).
  const std::string DataDir( "/home/mueller/Dropbox/Projects/NobleG4Scripts/Fitting/data/" );

  std::string tmp;
  
  // Load the G4 data sources.
  std::cout << "Loading G4 dataset: " << "Muons" << "..." << std::endl;
  G4Input Muons( "Muons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Muon_10keV.root" );
  G4Inputs["Muons"] = G4Input( "Muons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Muon_10keV.root" );
  std::cout << "Loading G4 dataset: " << "Protons" << "..." << std::endl;
  G4Inputs["Protons"] = G4Input( "Protons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Proton_10keV.root" );
  std::cout << "Loading G4 dataset: " << "Deuterons" << "..." << std::endl;
  G4Inputs["Deuterons"] = G4Input( "Deuterons", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/Deuteron_10keV.root" );
  std::cout << "Loading G4 dataset: " << "113Sn_ceK2_364keV" << "..." << std::endl;
  G4Inputs["DM_113Sn_ceK2_364keV"] = G4Input( "113Sn_ceK2_364keV", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/113Sn_ceK2_364keV.root" );
  std::cout << "Loading G4 dataset: " << "207Bi_ceK3_976keV" << "..." << std::endl;
  G4Inputs["DM_207Bi_ceK3_976keV"] = G4Input( "207Bi_ceK3_976keV", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/207Bi_ceK3_976keV.root" );
  std::cout << "Finished loading the G4 datasets." << std::endl;
  G4Inputs["DM_241Am_gamma_59keV"] = G4Input( "241Am_gamma_59keV", "/home/mueller/Projects/SimpleNobleG4Detector/G4/build/241Am_gamma_59keV.root" );
  std::cout << "Finished loading the G4 datasets." << std::endl;

  // Configure the ARIS 2018 DM dataset.
  tmp = "ARIS_241Am_gamma_59keV";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "ARIS2018_241Am_gamma_59keV_LY.csv" );
  DMDatasets[tmp].SetQorL( false );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_241Am_gamma_59keV"] );
  Active[tmp] = true;

  // Configure the Bondar 2016 DM dataset.
  tmp = "Bondar_241Am_gamma_59keV";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Bondar2016_241Am_gamma_59keV_QY.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_241Am_gamma_59keV"] );
  Active[tmp] = true;
  
  // Configure the Scalettar 1982 DM dataset (whole).
  tmp = "Scalettar_113Sn_ceK2_364keV";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Scalettar1982_113Sn_ceK2_364keV_QY.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_113Sn_ceK2_364keV"] );
  Active[tmp] = true;

  // Configure the Scalettar 1982 DM dataset (part I).
  tmp = "Scalettar_113Sn_ceK2_364keV_Part1";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Scalettar1982_113Sn_ceK2_364keV_QY_Part1.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_113Sn_ceK2_364keV"] );
  Active[tmp] = true;

  // Configure the Scalettar 1982 DM dataset (part II).
  tmp = "Scalettar_113Sn_ceK2_364keV_Part2";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Scalettar1982_113Sn_ceK2_364keV_QY_Part2.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_113Sn_ceK2_364keV"] );
  Active[tmp] = true;

  // Configure the Scalettar 1982 DM dataset (part III).
  tmp = "Scalettar_113Sn_ceK2_364keV_Part3";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Scalettar1982_113Sn_ceK2_364keV_QY_Part3.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_113Sn_ceK2_364keV"] );
  Active[tmp] = true;

  // Configure the Scalettar 1982 DM dataset (part IV).
  tmp = "Scalettar_113Sn_ceK2_364keV_Part4";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Scalettar1982_113Sn_ceK2_364keV_QY_Part4.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_113Sn_ceK2_364keV"] );
  Active[tmp] = true;

  // Configure the Aprile 1987 DM dataset.
  tmp = "Aprile_207Bi_ceK3_976keV";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  DMDatasets[tmp] = DMDataset( tmp, DataDir + "Aprile1987_207Bi_ceK3_976keV_QY.csv" );
  DMDatasets[tmp].SetQorL( true );
  DMDatasets[tmp].SetG4Input( &G4Inputs["DM_207Bi_ceK3_976keV"] );
  Active[tmp] = true;

  // Configure the ICARUS 3-Ton muon dataset.
  tmp = "ICARUS_3Ton_Muons";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ICARUS_3Ton_muons_absolute.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Muons"] );
  Active[tmp] = true;
  
  // Configure the ICARUS 3-Ton proton dataset.
  tmp = "ICARUS_3Ton_Protons";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ICARUS_3Ton_protons_absolute.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Protons"] );
  Active[tmp] = true;

  // Configure the ICARUS 3-Ton muon dataset (ONLY 500 V/cm POINTS).
  tmp = "ICARUS_3Ton_Muons_500";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ICARUS_3Ton_muons_Only500Vcm.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Muons"] );
  Active[tmp] = true;
  
  // Configure the ICARUS 3-Ton proton dataset (ONLY 500 V/cm POINTS).
  tmp = "ICARUS_3Ton_Protons_500";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ICARUS_3Ton_protons_Only500Vcm.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Protons"] );
  Active[tmp] = true;

  // Configure the ICARUS T600 muon dataset.
  tmp = "ICARUS_T600_Muons";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ICARUS_T600_muons.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Muons"] );
  Active[tmp] = true;

  // Configure the ArgoNeuT proton dataset.
  tmp = "ArgoNeuT_Protons";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ArgoNeuT_protons.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Protons"] );
  Active[tmp] = true;
  
  // Configure the ArgoNeuT deuteron dataset.
  tmp = "ArgoNeuT_Deuterons";
  std::cout << "Loading physics dataset: " << tmp << "..." << std::endl;
  NuDatasets[tmp] = NuDataset( tmp, DataDir + "ArgoNeuT_deuterons.csv" );
  NuDatasets[tmp].SetNBins( 100 );
  NuDatasets[tmp].SetBinL( 0.5 );
  NuDatasets[tmp].SetG4Input( &G4Inputs["Deuterons"] );
  Active[tmp] = true;

  std::cout << "Finished loading physics datasets." << std::endl;
}

void RegContainer::Eval( std::string Suffix, std::vector<double> &Parameters, const bool GenerateSummary )
{
  // Constants that I'm too lazy to organize better.
  const size_t NPar(3);
  const std::string Output( "RegressionSummary_" + Suffix + ".root" );
  
  // Variables for filling the TTrees.
  TFile *SummaryFile = TFile::Open( Output.c_str(), "recreate" );
  double p[NPar];
  double chi2;

  // Vector for storing the chi^2 values (passed to Eval functions). 
  std::vector<double> Chi2s( Parameters.size() / NPar );

  // Loop over the configured datasets and do the chi^2 calculations.
  auto Start = std::chrono::high_resolution_clock::now();

  std::map<std::string, DMDataset>::iterator DMIt;
  for( DMIt = DMDatasets.begin(); DMIt != DMDatasets.end(); ++DMIt)
  {
    // The key for the dataset provides an easy way to link between maps.
    auto &tmp = DMIt->first;

    // If the dataset is not marked active, then don't process it.
    if( Active[tmp] )
    {
      std::cout << "Dataset: " << tmp << " marked active. Processing..." << std::endl;
      Trees[tmp] = new TTree( tmp.c_str(), tmp.c_str() );
      for( size_t j(0); j < NPar; ++j ) Trees[tmp]->Branch( ("p" + std::to_string(j)).c_str(), p+j );
      Trees[tmp]->Branch( "chi2", &chi2 );
      DMIt->second.Eval( Parameters, Chi2s, GenerateSummary );
      for( size_t i(0); i < Chi2s.size(); ++i)
      {
	for( size_t pi(0); pi < NPar; ++pi ) p[pi] = Parameters[ NPar * i + pi ];
	chi2 = Chi2s[i];
	Trees[tmp]->Fill();
      }
      Trees[tmp]->Write();
    }
    else std::cout << "Dataset: " << tmp << " marked inactive. Skipping..." << std::endl;
  }

  std::map<std::string, NuDataset>::iterator NuIt;
  for( NuIt = NuDatasets.begin(); NuIt != NuDatasets.end(); ++NuIt)
  {
    // The key for the dataset provides an easy way to link between maps.
    auto &tmp = NuIt->first;

    // If the dataset is not marked active, then don't process it.
    if( Active[tmp] )
    {
      std::cout << "Dataset: " << tmp << " marked active. Processing..." << std::endl;
      Trees[tmp] = new TTree( tmp.c_str(), tmp.c_str() );
      for( size_t j(0); j < NPar; ++j ) Trees[tmp]->Branch( ("p" + std::to_string(j)).c_str(), p+j );
      Trees[tmp]->Branch( "chi2", &chi2 );
      NuIt->second.Eval( Parameters, Chi2s, GenerateSummary );
      for( size_t i(0); i < Chi2s.size(); ++i)
      {
	for( size_t pi(0); pi < NPar; ++pi ) p[pi] = Parameters[ NPar * i + pi ];
	chi2 = Chi2s[i];
	Trees[tmp]->Fill();
      }
      Trees[tmp]->Write();
    }
    else std::cout << "Dataset: " << tmp << " marked inactive. Skipping..." << std::endl;
  }
  
  SummaryFile->Close();
  delete SummaryFile;

  // Report the time statistic.
  auto Stop = std::chrono::high_resolution_clock::now();
  auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(Stop - Start);
  std::cout << "Time taken by function call: "
            << Duration.count()/1000000.0 << " seconds" << std::endl;
}

void RegContainer::SetActive( const std::string DatasetName, const bool IsActive )
{
  const auto &Target = Active.find( DatasetName );
  if( Target != Active.end() ) Active[ DatasetName ] = IsActive;
  else std::cout << "Target dataset " << DatasetName << " does not exist. Cannot toggle active state!" << std::endl;
}
