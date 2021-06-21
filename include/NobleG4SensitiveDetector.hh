#ifndef NobleG4SensitiveDetector_h
#define NobleG4SensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "NobleG4Hit.hh"

class G4Step;
class G4HCofThisEvent;

class NobleG4SensitiveDetector : public G4VSensitiveDetector
{
public:
  // Constructor/destructor for the sensitive volume.
  NobleG4SensitiveDetector(G4String SDName);
  ~NobleG4SensitiveDetector();

  // Member functions for the sensitive volumen.
  G4bool ProcessHits(G4Step *Step, G4TouchableHistory*);
  void Initialize(G4HCofThisEvent* HCE);
  void EndOfEvent(G4HCofThisEvent*);

private:
  NobleG4HitCollection* HitCollection;
};

#endif
