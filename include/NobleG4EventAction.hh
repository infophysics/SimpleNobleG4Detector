#ifndef NobleG4EventAction_h
#define NobleG4EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class NobleG4RunAction;

// Event action class.

class NobleG4EventAction : public G4UserEventAction
{
  public:
    NobleG4EventAction(NobleG4RunAction* RunAction);
    virtual ~NobleG4EventAction();

    virtual void BeginOfEventAction(const G4Event* Event);
    virtual void EndOfEventAction(const G4Event* Event);

    void AddEnergy(G4double Energy) { fEnergy += Energy; }
    void AddElectrons(G4double Electrons) { fElectrons += Electrons; }
    void AddPhotons(G4double Photons) { fPhotons += Photons; }
    G4bool GetTupleState();
  
  private:
    NobleG4RunAction* fRunAction;
    G4double fEnergy;
    G4double fElectrons;
    G4double fPhotons;
};

#endif

    
