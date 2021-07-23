// C++ includes.
#include <string>
//#include <chrono>
#include <vector>

// ROOT includes.
//#include "TF2.h"
//#include "TFile.h"

// Custom includes.
#include "RegContainer.hh"
//#include "NuDataset.hh"
//#include "DMDataset.hh"
//#include "G4Input.hh"
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

  RegContainer Regression;
  Regression.SetActive( "ARIS_241Am_gamma_59keV", false );
  Regression.SetActive( "Bondar_241Am_gamma_59keV", false );
  Regression.SetActive( "Scalettar_113Sn_ceK2_364keV", false );
  Regression.SetActive( "Scalettar_113Sn_ceK2_364keV_Part1", false );
  Regression.SetActive( "Scalettar_113Sn_ceK2_364keV_Part2", false );
  Regression.SetActive( "Scalettar_113Sn_ceK2_364keV_Part3", false );
  Regression.SetActive( "Scalettar_113Sn_ceK2_364keV_Part4", false );
  Regression.SetActive( "Aprile_207Bi_ceK3_976keV", false );
  Regression.SetActive( "ICARUS_3Ton_Muons", false );
  Regression.SetActive( "ICARUS_3Ton_Protons", true );
  Regression.SetActive( "ICARUS_3Ton_Muons_500", false );
  Regression.SetActive( "ICARUS_3Ton_Protons_500", false );
  Regression.SetActive( "ICARUS_T600_Muons", true );
  Regression.SetActive( "ArgoNeuT_Protons", false );
  Regression.SetActive( "ArgoNeuT_Deuterons", false );
  
  
  Configuration Config( "Birks 3D" );
  const size_t NPar(3);
  Config.SetCubeDim(NPar);
  Config.SetCubeLen( { 0.001000, 0.00030 } );
  Config.SetCubeCen( { 0.915500, 0.09 } );
  Config.SetCubeN( 20 );
  std::vector<double> Parameters = Config.GetParameterGrid();

  Regression.Eval( "Jul23_2", Parameters, false );

  return 0;
}
