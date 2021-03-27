#ifndef NobleG4SteppingAction_h
#define NobleG4SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class NobleG4EventAction;

class G4LogicalVolume;

/// Stepping action class

class NobleG4SteppingAction : public G4UserSteppingAction
{
  public:
    NobleG4SteppingAction(NobleG4EventAction* eventAction);
    virtual ~NobleG4SteppingAction();

    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

  private:
    NobleG4EventAction* fEventAction;
    G4LogicalVolume* fSensitiveVolume;
};

#endif
