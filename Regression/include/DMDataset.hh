#ifndef DMDataset_hh
#define DMDataset_hh

// C++ includes.
#include <vector>

// Custom includes.
#include "Dataset.hh"

class DMDataset : public Dataset
{
public:
  using Dataset::Dataset;
  void Eval( const std::vector<double> &Parameters, std::vector<double> &Chi2s, const bool GenerateSummary=false );
  void SetQorL( const bool QorL ) { IsQY = QorL; }
  std::vector<double> GetFields();

private:
  bool IsQY;
};


#endif
