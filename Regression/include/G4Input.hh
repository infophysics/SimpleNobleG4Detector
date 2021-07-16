#ifndef G4Input_hh
#define G4Input_hh

// C++ includes.
#include <vector>
#include <string>

class G4Input
{
public:
  G4Input( const std::string G4Name, const std::string G4File );
  const std::vector<int>& GetG4N() const { return G4N; }
  const std::vector<int>& GetG4B() const { return G4B; }
  const std::vector<double>& GetG4dE() const { return G4dE; }
  const std::vector<double>& GetG4dx() const { return G4dx; }

private:
  std::string Name;
  std::vector<int> G4N;
  std::vector<int> G4B;
  std::vector<double> G4dE;
  std::vector<double> G4dx;
};

#endif
