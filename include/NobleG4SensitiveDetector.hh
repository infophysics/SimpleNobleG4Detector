#include "G4VSensitiveDetector.hh"

class NobleG4SensitiveDetector : public G4VSensitiveDetector
{
public:
  // Constructor/destructor for the sensitive volume.
  NobleG4SensitiveDetector(G4String SDName);
  ~NobleG4SensitiveDetector();

  // Member functions for the sensitive volumen.
  G4bool ProcessHits(G4Step *Step, G4TouchableHistory *THist);
  void Initialize(G4HCofThisEvent* HCE);
  void EndOfEvent(G4HCofThisEvent* HCE);
};
