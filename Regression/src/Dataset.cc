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

Dataset::Dataset( const std::string DatasetName, const std::string DataFileName )
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
