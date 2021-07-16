// C++ includes.
#include <vector>
#include <algorithm>

// ROOT includes.
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

// Custom includes.
#include "NuDataset.hh"
#include "EvalCUDA.hh"
#include "G4Input.hh"

void NuDataset::Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s, const bool GenerateSummary )
{
  const size_t PSize( Parameters.size() / 2 );
  const size_t NFields( UniqueFields.size() );
  
  std::vector< std::vector<double> > dEBins(NFields, std::vector<double>(PSize * NBins, 0 ) );
  std::vector< std::vector<double> > RBins(NFields, std::vector<double>(PSize * NBins, 0 ) );

  ReCUDA::BirksWrapper( G4Data->GetG4dE(), G4Data->GetG4dx(),
			G4Data->GetG4B(), UniqueFields,
			Parameters, RBins, dEBins );

  double chi2(10000);
  TGraph* Graph;
  TFile *Summary = nullptr;
  if( GenerateSummary ) Summary = TFile::Open(std::string(Name+"_Summary.root").c_str(), "recreate");
  std::cout << "Lowest dE/dx: " << dEBins.at(0).back() << std::endl;
  for( size_t pid(0); pid < PSize; ++pid )
  {
    chi2 = 0;
    for( size_t f(0); f < NFields; ++f )
    {
      Graph = new TGraph(NBins, &dEBins.at(f)[ NBins * pid ], &RBins.at(f)[ NBins * pid ]);
      for( size_t i(0); i < x.size(); ++i)
      {
	if( y.at(i) == UniqueFields.at(f) && x.at(i) > dEBins.at(f).back() ) chi2 += std::pow(Graph->Eval(x.at(i)) - z.at(i), 2) / std::pow(sz.at(i), 2);
      }

      if( pid == 0 && GenerateSummary )
      {
	TTree *SummaryTree = new TTree(std::string("Field" + std::to_string(f)).c_str(), "tree");
	double XVal, ZVal;
	SummaryTree->Branch("dEdx", &XVal);
	SummaryTree->Branch("R", &ZVal);
	for( size_t j(0); j < 1000; ++j )
	{
	  XVal = dEBins.at(f).back() + j*(dEBins.at(f).at(0) - dEBins.at(f).back())/1000;
	  ZVal = Graph->Eval(XVal);
	  SummaryTree->Fill();
	}
	Summary->Write();
	delete SummaryTree;
      }
    }
    Chi2s[pid] = chi2;
  }
  if( GenerateSummary ) Summary->Close();
}
