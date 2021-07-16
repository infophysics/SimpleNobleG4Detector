#ifndef NobleG4EventAction_h
#define NobleG4EventAction_h 1

#include "NobleG4Hit.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

class NobleG4RunAction;

// Event action class.

class NobleG4EventAction : public G4UserEventAction
{
public:
  NobleG4EventAction(NobleG4RunAction* RunAction);
  virtual ~NobleG4EventAction();

  NobleG4HitCollection* GetHitsCollection(G4int HCID, const G4Event* Event) const;

  virtual void BeginOfEventAction(const G4Event* Event);
  virtual void EndOfEventAction(const G4Event* Event);
  
  void AddEnergy(G4double Energy) { fEnergy += Energy; }
  void AddElectrons(G4double Electrons) { fElectrons += Electrons; }
  void AddPhotons(G4double Photons) { fPhotons += Photons; }
  G4bool GetEventOutput();
  G4bool GetStepOutput();
  G4bool GetTrackingOutput();
  G4double GetField();
  
private:
  NobleG4RunAction* fRunAction;
  G4int fHCID;
  G4double fEnergy;
  G4double fElectrons;
  G4double fPhotons;
};

#endif

    
