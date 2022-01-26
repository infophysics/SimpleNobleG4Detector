#ifndef RegContainer_hh
#define RegContainer_hh

// C++ includes.
#include <string>
#include <map>

// ROOT includes.
#include "TTree.h"

// Custom includes.
#include "NuDataset.hh"
#include "DMDataset.hh"
#include "G4Input.hh"

class RegContainer
{
public:
  RegContainer();
  void Eval( std::string Suffix, std::vector<double> &Parameters, const bool GenerateSummary );
  void SetActive( const std::string DatasetName, const bool IsActive );

private:
  std::map<std::string, G4Input> G4Inputs;
  std::map<std::string, NuDataset> NuDatasets;
  std::map<std::string, DMDataset> DMDatasets;
  std::map<std::string, TTree*> Trees;
  std::map<std::string, bool> Active;
};

#endif
