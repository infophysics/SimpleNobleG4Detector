#ifndef Configuration_hh
#define Configuration_hh

// C++ includes.
#include <vector>
#include <string>

class Configuration
{
public:
  Configuration( std::string ConfigName );
  void SetCubeDim( const int N ) { NDim = N; };
  void SetCubeLen( const std::vector<double> L ) { CubeL = L; }
  void SetCubeCen( const std::vector<double> C ) { CubeC = C; }
  void SetCubeN( const size_t N ) { CubeN = N; }
  std::vector<double>& GetParameterGrid();

private:
  std::string Name;
  int NDim;
  std::vector<double> CubeL;
  std::vector<double> CubeC;
  size_t CubeN;
  std::vector<double> ParameterGrid;
};

#endif
