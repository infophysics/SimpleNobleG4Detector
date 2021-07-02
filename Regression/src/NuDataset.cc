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

double NuDataset::Eval( const TF2* BareReco, const bool GenerateSummary)
{
  size_t NFields = UniqueFields.size();
  std::vector< std::vector<double> > dQBins(NFields, std::vector<double>(NBins, 0));
  std::vector< std::vector<double> >  dEBins(NFields, std::vector<double>(NBins, 0));
  std::vector< std::vector<double> >  R(NFields, std::vector<double>(NBins, 0));

  double dEdx(0);
  double StepdQ(0);
  unsigned int NEvents(0);
  TTreeReader Reader(G4Tree);
  TTreeReaderValue<Int_t> TupleN(Reader, "N");
  TTreeReaderValue<Int_t> TupleB(Reader, "B");
  TTreeReaderValue<Double_t> TupledE(Reader, "dE");
  TTreeReaderValue<Double_t> Tupledx(Reader, "dx");

  while( Reader.Next() )
  {
    dEdx = *TupledE / *Tupledx;
    for( size_t f(0); f < NFields; ++f)
    {
      StepdQ = BareReco->Eval( dEdx, UniqueFields.at(f) ) * (*Tupledx);
      if( !std::isnan(StepdQ) ) dQBins.at(f).at(*TupleB) += StepdQ;
      dEBins.at(f).at(*TupleB) += *TupledE;
      if( *TupleN > NEvents ) NEvents = *TupleN;
    }
  }


  double chi2(0);
  TGraph* Graph;
  TFile *Summary;
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
    //std::cout << "Chi^2 after field " << UniqueFields.at(f) << " V/cm: " << chi2 << "." << std::endl;
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
  
  
  /*TCanvas *Canvas = new TCanvas("c1", "Graph", 200, 10, 700, 500);
  TGraphErrors *GErr = new TGraphErrors(22, &x[0], &z[0], 0, &sz[0]);
  GErr->SetTitle("ICARUS 500 V/cm Muons");
  GErr->Draw("APL");

  double XVal[1000], ZVal[1000];
  for( size_t j(0); j < 1000; ++j )
  {
    XVal[j] = 2+j*(13.0/1000);//dEBins.back() + i*(dEBins.at(0) - dEBins.back())/1000;
    ZVal[j] = Graph->Eval(XVal[j], 0, "S");
    if( j % 10 == 0 ) std::cout << "(" << XVal[j] << "," << ZVal[j] << ") ";
  }
  TGraph *SmoothGraph = new TGraph(1000, XVal, ZVal);
  SmoothGraph->GetXaxis()->SetRangeUser(0, 20);
  SmoothGraph->GetYaxis()->SetRangeUser(0, 1);
  SmoothGraph->Draw("AP");

  TFile *Output = new TFile("output.root", "RECREATE");
  Canvas->Write();
  GErr->Write();
  SmoothGraph->Write();
  Output->Close();*/

  return chi2;
}
