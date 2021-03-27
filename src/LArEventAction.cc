#include "LArEventAction.hh"
#include "LArRunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

LArEventAction::LArEventAction(LArRunAction* RunAction)
: G4UserEventAction(),
  fRunAction(RunAction),
  fEdep(0.)
{} 

LArEventAction::~LArEventAction()
{}

void LArEventAction::BeginOfEventAction(const G4Event*)
{    
  fEdep = 0.;
}

void LArEventAction::EndOfEventAction(const G4Event*)
{   
  // Accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
}
