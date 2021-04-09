#include "NobleG4EventAction.hh"
#include "NobleG4RunAction.hh"
#include "NobleG4Analysis.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

NobleG4EventAction::NobleG4EventAction(NobleG4RunAction* RunAction)
: G4UserEventAction(),
  fRunAction(RunAction),
  fEnergy(0.0),
  fElectrons(0.0),
  fPhotons(0.0)
{} 

NobleG4EventAction::~NobleG4EventAction()
{}

void NobleG4EventAction::BeginOfEventAction(const G4Event*)
{    
  fEnergy = 0.0;
  fElectrons = 0.0;
  fPhotons = 0.0;
}

void NobleG4EventAction::EndOfEventAction(const G4Event* Event)
{   
  // Accumulate statistics in the Run Action.
  fRunAction->AddEnergy(fEnergy);
  fRunAction->AddElectrons(fElectrons);
  fRunAction->AddPhotons(fPhotons);

  // Populate (event-level) analysis n-tuple.
  if(this->GetTupleState()) PopulateEventTuple(Event->GetEventID(), fEnergy, fElectrons, fPhotons);
}

G4bool NobleG4EventAction::GetTupleState()
{
  
  return fRunAction->GetTupleState();
}
