#ifndef Dataset_hh
#define Dataset_hh

// C++ includes.
#include <vector>
#include <string>

// ROOT includes.
#include "TF2.h"
#include "TFile.h"
#include "TTree.h"

// Custom includes.
#include "G4Input.hh"

class Dataset
{
public:
  Dataset( const std::string DatasetName, const std::string DataFileName );
  Dataset();
  virtual void Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s, const bool GenerateSummary=false ) = 0;
  void SetN( const size_t N);
  void SetG4Input( G4Input* G4 ) { G4Data = G4; }

  std::vector<double> X() const { return x; }
  std::vector<double> Y() const { return y; }
  std::vector<double> Z() const { return z; }
  std::vector<double> sX() const { return sx; }
  std::vector<double> sY() const { return sy; }
  std::vector<double> sZ() const { return sz; }
  
  
protected:
  std::string Name;
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  std::vector<double> sx;
  std::vector<double> sy;
  std::vector<double> sz;
  std::vector<double> UniqueFields;

  G4Input* G4Data;
  
};
#endif
