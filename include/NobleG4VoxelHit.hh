#include "G4VHit.hh"
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"

class NobleG4VoxelHit : public G4VHit
{
public:
  NobleG4VoxelHit(const G4ThreeVector VoxID);
  ~NobleG4VoxelHit();

  void Print();
  void AddEDep(const double e){ EDep += e; }
  G4double GetEDep() const { return EDep; }
  G4ThreeVector GetVoxID() const { return VoxID; }

private:
  const G4ThreeVector VoxID;
  G4double EDep;
};

typedef G4THitsCollection<NobleG4VoxelHit> NobleG4VoxelHitCollection;
