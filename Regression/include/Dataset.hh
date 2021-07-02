// C++ includes.
#include <vector>
#include <string>

// ROOT includes.
#include "TF2.h"
#include "TFile.h"
#include "TTree.h"

class Dataset
{
public:
  Dataset(const std::string DatasetName, const std::string DataFileName, const std::string G4FileName );
  virtual double Eval( const TF2* BareReco, const bool GenerateSummary=false ) = 0;
  void SetN( const size_t N);

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

  TFile *G4File;
  TTree *G4Tree;
  
};
