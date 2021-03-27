#include "LArActionInitialization.hh"
#include "LArPrimaryGeneratorAction.hh"
#include "LArRunAction.hh"
#include "LArEventAction.hh"
#include "LArSteppingAction.hh"

LArActionInitialization::LArActionInitialization()
 : G4VUserActionInitialization()
{}

LArActionInitialization::~LArActionInitialization()
{}

void LArActionInitialization::BuildForMaster() const
{
  LArRunAction* RunAction = new LArRunAction;
  SetUserAction(RunAction);
}

void LArActionInitialization::Build() const
{
  SetUserAction(new LArPrimaryGeneratorAction);

  LArRunAction* RunAction = new LArRunAction;
  SetUserAction(RunAction);
  
  LArEventAction* EventAction = new LArEventAction(RunAction);
  SetUserAction(EventAction);
  
  SetUserAction(new LArSteppingAction(EventAction));
}
