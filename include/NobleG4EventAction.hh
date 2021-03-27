#ifndef NobleG4EventAction_h
#define NobleG4EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class NobleG4RunAction;

/// Event action class.

class NobleG4EventAction : public G4UserEventAction
{
  public:
    NobleG4EventAction(NobleG4RunAction* runAction);
    virtual ~NobleG4EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }

  private:
    NobleG4RunAction* fRunAction;
    G4double     fEdep;
};

#endif

    
