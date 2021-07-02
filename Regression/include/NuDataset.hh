// C++ includes.
#include <vector>

// ROOT includes.
#include "TF2.h"

// Custom includes.
#include "Dataset.hh"


class NuDataset : public Dataset
{
public:
  using Dataset::Dataset;
  double Eval( const TF2* BareReco, const bool GenerateSummary=false);
  void SetNBins( const unsigned int N ) { NBins = N; }
  void SetBinL( const float L ) { BinL = L; }
  std::vector<double> GetFields();
  
private:
  unsigned int NBins;
  float BinL;
};
