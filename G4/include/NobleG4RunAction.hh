#ifndef NobleG4RunAction_h
#define NobleG4RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

class G4Run;

// The Run Action class.
// In EndOfRunAction(), the total energy deposited
// in the active volume during the run is calculated
// and printed to the screen.

class NobleG4RunAction : public G4UserRunAction
{
public:
  NobleG4RunAction();
  virtual ~NobleG4RunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
  
  void AddEnergy(G4double Energy);
  void AddElectrons(G4double Electrons);
  void AddPhotons(G4double Photons);
  G4bool GetEventOutput() { return fEventOutput; }
  G4bool GetStepOutput() { return fStepOutput; }
  G4bool GetTrackingOutput() { return fTrackingOutput; }
  void SetOutput(G4String Val);
  G4double GetField() { return fField; }
  void SetField(G4String Val);
  
private:
  G4Accumulable<G4double> fEnergy;
  G4Accumulable<G4double> fElectrons;
  G4Accumulable<G4double> fPhotons;
  G4GenericMessenger* fMessenger;
  G4bool fEventOutput;
  G4bool fStepOutput;
  G4bool fTrackingOutput;
  G4double fField;
};

#endif

