#include "NobleG4EventAction.hh"
#include "NobleG4RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

NobleG4EventAction::NobleG4EventAction(NobleG4RunAction* RunAction)
: G4UserEventAction(),
  fRunAction(RunAction),
  fEdep(0.)
{} 

NobleG4EventAction::~NobleG4EventAction()
{}

void NobleG4EventAction::BeginOfEventAction(const G4Event*)
{    
  fEdep = 0.;
}

void NobleG4EventAction::EndOfEventAction(const G4Event*)
{   
  // Accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
}
