// C++ includes.
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

// ROOT includes.
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// Custom includes.
#include "Dataset.hh"

Dataset::Dataset(const std::string DatasetName, const std::string DataFileName, const std::string G4FileName)
{
  Name = DatasetName;
  std::ifstream File;
  std::string Line;
  char c;
  size_t LineCount(0), n(0);
  File.open(DataFileName);
  if( File.is_open() )
  {
    LineCount = std::count(std::istreambuf_iterator<char>(File), std::istreambuf_iterator<char>(), '\n');
    this->SetN(LineCount);
    File.clear();
    File.seekg(0);
    while( getline(File, Line) )
    {
      std::istringstream Stream(Line);
      Stream >> x.at(n) >> c
	     >> sx.at(n) >> c
	     >> y.at(n) >> c
	     >> sy.at(n) >> c
	     >> z.at(n) >> c
	     >> sz.at(n);
      n++;
    }
    for( const auto& f : y )
    {
    if( std::find(UniqueFields.begin(), UniqueFields.end(), f) == UniqueFields.end() ) UniqueFields.push_back(f);
    }
  }
  else std::cerr << "Error opening input file: " << DataFileName << "." << std::endl;

  TFile *G4File = new TFile(G4FileName.c_str());
  TTree *G4Tree = (TTree*)G4File->Get("G4Tree");

  TTreeReader Reader(G4Tree);
  TTreeReaderValue<Int_t> TupleN(Reader, "N");
  TTreeReaderValue<Int_t> TupleB(Reader, "B");
  TTreeReaderValue<Double_t> TupledE(Reader, "dE");
  TTreeReaderValue<Double_t> Tupledx(Reader, "dx");

  int G4Entries(Reader.GetEntries());
  size_t m(0);
  G4N.resize(G4Entries);
  G4B.resize(G4Entries);
  G4dE.resize(G4Entries);
  G4dx.resize(G4Entries);
  
  while( Reader.Next() )
  {
    G4N[m] = *TupleN;
    G4B[m] = *TupleB;
    G4dE[m] = *TupledE;
    G4dx[m] = *Tupledx;
    ++m;
  }

  G4File->Close();
}

void Dataset::SetN(const size_t N)
{
  x.resize(N);
  y.resize(N);
  z.resize(N);
  sx.resize(N);
  sy.resize(N);
  sz.resize(N);
}
