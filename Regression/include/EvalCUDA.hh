#ifndef EvalCUDA_hh
#define EvalCUDA_hh

// C++ includes.
#include <vector>

namespace ReCUDA
{
  void BirksWrapper( const std::vector<double> &dE, const std::vector<double> &dx,
                     const std::vector<int> &B, const std::vector<double> &F,
                     const std::vector<double> &p, std::vector< std::vector<double> >  &RBins,
		     std::vector< std::vector<double> >  &dEBins );
  void DMBirksWrapper( const std::vector<double> &dE, const std::vector<double> &dx,
		       const std::vector<double> &F, const std::vector<double> &p,
		       std::vector< std::vector<double> >  &QY );
}

#endif
