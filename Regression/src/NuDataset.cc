// C++ includes.
#include <vector>
#include <algorithm>

// ROOT includes.
#include "TF2.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"

// Custom includes.
#include "NuDataset.hh"
#include "EvalCUDA.hh"

//double NuDataset::Eval( const TF2* BareReco, const bool GenerateSummary)
void NuDataset::Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s ) //const bool GenerateSummary )
{
  const size_t PSize(Parameters.size()/2);
  const size_t NFields = UniqueFields.size();
  
  std::vector< std::vector<double> > dEBins(NFields, std::vector<double>(PSize * NBins, 0 ) );
  std::vector< std::vector<double> > RBins(NFields, std::vector<double>(PSize * NBins, 0 ) );


  ReCUDA::BirksWrapper( G4dE, G4dx, G4B, UniqueFields, Parameters, RBins, dEBins );

  double chi2(10000);
  TGraph* Graph;
  for( size_t pid(0); pid < PSize; ++pid )
  {
    for( size_t f(0); f < NFields; ++f )
    {
      chi2 = 0;
      Graph = new TGraph(NBins, &dEBins.at(f)[ NBins * pid ], &RBins.at(f)[ NBins * pid ]);
      for( size_t i(0); i < x.size(); ++i)
      {
	if( y.at(i) == UniqueFields.at(f) && x.at(i) > dEBins.at(f).back() ) chi2 += std::pow(Graph->Eval(x.at(i)) - z.at(i), 2) / std::pow(sz.at(i) * z.at(i), 2);
      }
    }
    Chi2s[pid] = chi2;
  }
  
  // Return a vector of chi^2 values (over the parameter sets).
  
  /*size_t NFields = UniqueFields.size();
  std::vector< std::vector<double> > dQBins(NFields, std::vector<double>(NBins, 0));
  std::vector< std::vector<double> >  dEBins(NFields, std::vector<double>(NBins, 0));
  std::vector< std::vector<double> >  R(NFields, std::vector<double>(NBins, 0));

  double dEdx(0);
  double StepdQ(0);
  int NEvents(0);

  for( size_t i(0); i < G4N.size(); ++i )
  {
    dEdx = G4dE[i] / G4dx[i];
    for( size_t f(0); f < NFields; ++f )
    {
      StepdQ = BareReco->Eval( dEdx, UniqueFields.at(f) ) * G4dx[i];
      if( !std::isnan(StepdQ) ) dQBins.at(f).at(G4B[i]) += StepdQ;
      dEBins.at(f).at(G4B[i]) += G4dE[i];
      if( G4N[i] > NEvents ) NEvents = G4N[i];
    }
  }

  double chi2(0);
  TGraph* Graph;
  TFile *Summary = nullptr;
  if( GenerateSummary ) Summary = TFile::Open(std::string(Name+"_Summary.root").c_str(), "recreate");

  for( size_t f(0); f < NFields; ++f)
  {
    std::transform(dQBins.at(f).begin(), dQBins.at(f).end(), dQBins.at(f).begin(), std::bind(std::divides<float>(), std::placeholders::_1, NEvents*BinL));
    std::transform(dEBins.at(f).begin(), dEBins.at(f).end(), dEBins.at(f).begin(), std::bind(std::divides<float>(), std::placeholders::_1, NEvents*BinL));
    std::transform(dQBins.at(f).begin(), dQBins.at(f).end(), dEBins.at(f).begin(), R.at(f).begin(), std::divides<double>());
    
    Graph = new TGraph(NBins, &dEBins.at(f)[0], &R.at(f)[0]);

    for( size_t i(0); i < x.size(); ++i)
    {
      if( y.at(i) == UniqueFields.at(f) && x.at(i) > dEBins.at(f).back() ) chi2 += std::pow(Graph->Eval(x.at(i)) - z.at(i), 2) / std::pow(sz.at(i) * z.at(i), 2);
    }

    if( GenerateSummary )
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
  if( GenerateSummary ) Summary->Close();
  */
  
  //return chi2;
}
