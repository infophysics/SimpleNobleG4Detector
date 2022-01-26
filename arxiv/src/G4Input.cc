// C++ includes.
#include <vector>
#include <string>

// ROOT includes.
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

// Custom includes.
#include "G4Input.hh"

G4Input::G4Input()
{

}

G4Input::G4Input( const std::string G4Name, const std::string G4File )
{
	// May be useful to have a unique name for the G4 input.
	Name = G4Name;

	// The data is stored in a ROOT file, but it is not practical to load it
	// up each time we want it. We can save a significant amount of time by
	// loading it into local memory once. Using a TTreeReader object makes this
	// a bit simpler and cleaner.
	TFile *G4TFile = new TFile(G4File.c_str());
	TTree *G4Tree = (TTree*)G4TFile->Get("G4Tree");

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
	
	// Close the file (automatically releases memory of the TFile + associated TTree).
	G4TFile->Close();
}
