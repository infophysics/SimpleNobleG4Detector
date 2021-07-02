#include "NobleG4ActionInitialization.hh"
#include "NobleG4PrimaryGeneratorAction.hh"
#include "NobleG4RunAction.hh"
#include "NobleG4EventAction.hh"
#include "NobleG4SteppingAction.hh"

NobleG4ActionInitialization::NobleG4ActionInitialization()
 : G4VUserActionInitialization()
{}

NobleG4ActionInitialization::~NobleG4ActionInitialization()
{}

void NobleG4ActionInitialization::BuildForMaster() const
{
  NobleG4RunAction* RunAction = new NobleG4RunAction;
  SetUserAction(RunAction);
}

void NobleG4ActionInitialization::Build() const
{
  SetUserAction(new NobleG4PrimaryGeneratorAction);

  NobleG4RunAction* RunAction = new NobleG4RunAction;
  SetUserAction(RunAction);
  
  NobleG4EventAction* EventAction = new NobleG4EventAction(RunAction);
  SetUserAction(EventAction);
  
  SetUserAction(new NobleG4SteppingAction(EventAction));
}
