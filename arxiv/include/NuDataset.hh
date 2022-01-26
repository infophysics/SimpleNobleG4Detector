#ifndef NuDataset_hh
#define NuDataset_hh

// C++ includes.
#include <vector>

// Custom includes.
#include "Dataset.hh"


class NuDataset : public Dataset
{
public:
  using Dataset::Dataset;
  void Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s, const bool GenerateSummary=false );
  void SetNBins( const unsigned int N ) { NBins = N; }
  void SetBinL( const float L ) { BinL = L; }
  std::vector<double> GetFields();
  
private:
  unsigned int NBins;
  float BinL;
};

#endif
